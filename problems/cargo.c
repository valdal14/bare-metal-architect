#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define CARGO_QUEUE_CAPACITY 10
// CargoBay has only 4 rails to handle incoming Containers
#define CARGO_RAILS_CAPACITY 4
#define TC_SIZE 7 

#define BIT(x) (1 << (x))
#define IN_QUEUE_IDX 0 
#define INSPECTED_IDX 2
#define QUARANTINE_IDX 4
#define DEPARTURE_IDX 8
#define RESET_STATE 0x00 
#define DEFAULT_MAT_MASK 0x00 
#define MAT_L_MASK 0xF0
#define MAT_R_MASK 0x0F 
#define MAT_OFFSET 4
#define MAT_COMBINE(left, right) ((((uint8_t)(left) << MAT_OFFSET) & MAT_L_MASK) | ((uint8_t)(right) & MAT_R_MASK))
#define MAT_EXTRACT_LEFT(val) (((uint8_t)(val) & MAT_L_MASK) >> MAT_OFFSET)
#define MAT_EXTRACT_RIGHT(val) ((uint8_t)(val) & MAT_R_MASK)

typedef struct TrackingCode
{
    char *code;
    uint8_t size;
} TrackingCode;

typedef enum CargoMaterial
{
    MAT_BIO,
    MAT_MAC
} CargoMaterial;

typedef union Material
{
    /**
     * Biological States
     * Temperature: 4 Bits 
     * Oxygen:      4 Bits
     */
    uint8_t biological;
    /**
     * Machinery States 
     * Weight:      4 Bits
     * Voltage:     4 Bits
     */
    uint8_t machinery;
} Material;

typedef struct Container
{
    char *tracking_code;
    union Material container_material;
    /**
     * Container States
     * Queued:      Bit 0
     * Inspected:   Bit 2
     * Quarantined: Bit 4
     * Departure:   Bit 8
     */
    uint8_t container_state;
    struct Container *next;

} Container;

typedef struct CargoBay
{
    Container **rails;
    uint8_t capacity;
} CargoBay;

typedef struct CargoQueue
{
    Container *queue[CARGO_QUEUE_CAPACITY];
    uint8_t capacity;
    uint8_t size;
} CargoQueue;

/**
 * @brief Hashes a string key into a valid array index.
 * @param key The string to hash.
 * @param uint8_t capacity The size of the hash table array.
 * @return uint8_t The calculated index.
 */
uint8_t hash_function(const char *key, uint8_t capacity) {
    int hash = 0;
    while (*key != '\0') {
        hash = (hash + *key) % capacity;
        key++;
    }
    return hash;
}

/**
 * @brief Validates the incoming tracking code from the CargoBay
 * @param char tracking_code pointer
 * @return TrackingCode pointer
 */
TrackingCode *validate_tc(char *tracking_code)
{
    if(strlen(tracking_code) != TC_SIZE)
    {
        fprintf(stderr, "Invalid Tracking code detected\n");
        return NULL;
    }

    TrackingCode *tc = (TrackingCode *)calloc(1, sizeof(TrackingCode));
    uint8_t tc_size = strlen(tracking_code) + 1;
    tc->code = (char *)malloc(sizeof(char) * tc_size);
    
    if(tc == NULL || tc->code == NULL)
    {
        fprintf(stderr, "Could not generate tracking code label for container '%s'\n", tracking_code);
        return NULL;
    }
    
    strncpy(tc->code, tracking_code, tc_size);
    tc->code[tc_size - 1] = '\0';
    tc->size = tc_size;

    return tc;
}

/**
 * @brief Loads the dequeued Container to the CargoBay
 * @param CargoBay bay pointer
 * @param Container container pointer 
 * @return void
 */
void load(CargoBay *bay, Container *container)
{
    TrackingCode *tc = validate_tc(container->tracking_code);
    uint8_t idx = hash_function(tc->code, CARGO_RAILS_CAPACITY);

    if((container->container_state & BIT(INSPECTED_IDX)) == 0)
    {
        fprintf(stderr, "Expected Container's state to be INSPECTED. The container will be discarded\n");
        return;
    }

    if(bay->rails[idx] == NULL)
    {
        bay->rails[idx] = container;
    }
    else
    {
        Container *current = bay->rails[idx];
        while(current->next != NULL) current = current->next;
        container->container_state = RESET_STATE;
        container->container_state |= BIT(DEPARTURE_IDX);
        current->next = container;
    }

    free(tc);
}

/**
 * @brief Inspects the incoming container and change its state to INSPECTED
 * if the container is valid otherwise sets its state to QUARANTINE
 * If the container_state is in QUEUE state the container
 * can be loaded otherwise it will be placed in Quarantine.
 * @param Container container pointer
 * @param CargoMaterial mat
 * @return void
 */
void inspect(Container *container, CargoMaterial mat)
{
    int quarantine = 0;

    if((container->container_state & BIT(IN_QUEUE_IDX)) == 0)
    {
        fprintf(stderr, "Expected Container's state to be QUEUE. The container will be placed in Quarantine state\n");
        quarantine = 1;
    }

    switch(mat)
    {
        case MAT_BIO:
        {
            uint8_t b1 = MAT_EXTRACT_LEFT(container->container_material.biological);
            uint8_t b2 = MAT_EXTRACT_RIGHT(container->container_material.biological);
            if(b1 == 0 || b2 == 0) quarantine = 1;
            break;
        }
        case MAT_MAC:
        {    
            uint8_t m1 = MAT_EXTRACT_LEFT(container->container_material.machinery);
            uint8_t m2 = MAT_EXTRACT_RIGHT(container->container_material.machinery);
            if(m1 == 0 || m2 == 0) quarantine = 1;
            break;
        }
        default:
            fprintf(stderr, "Unidentified Material, the container will be placed in Quarantine state\n");
            quarantine = 1;
            break;
    }

    if(quarantine == 1)
    {
        container->container_state = RESET_STATE;
        container->container_state |= BIT(QUARANTINE_IDX);
    }
    else
    {
        container->container_state = RESET_STATE;
        container->container_state |= BIT(INSPECTED_IDX);
    }
}

/**
 * @brief Adds a new Container to the CargoQueue before the inspection phase
 * @param CargoQueue queue pointer
 * @param char code pointer
 * @param CargoMaterial mat 
 * @param uint8_t v1 
 * @param uint8_t v2
 * @return void
 */
void enqueue(CargoQueue *queue, char *code, CargoMaterial mat, uint8_t v1, uint8_t v2)
{
    if(queue->size + 1 > CARGO_QUEUE_CAPACITY)
    {
        fprintf(stderr, "Please standby, the CargoQueue is full\n");
        exit(1);
    }

    Container *container = (Container *)malloc(sizeof(Container));
    
    if(container == NULL)
    {
        fprintf(stderr, "Could not allocate space for the Container\n");
        return;
    }

    // Validates the incoming container
    TrackingCode *tc = validate_tc(code);
    container->tracking_code = (char *)malloc(sizeof(char) * tc->size);
    container->next = NULL;

    if(container->tracking_code == NULL)
    {
        fprintf(stderr, "Could not generate tracking code label for container '%s'\n", code);
        return;
    }

    strncpy(container->tracking_code, tc->code, tc->size);

    switch(mat)
    {
        case MAT_BIO:
            container->container_state |= BIT(IN_QUEUE_IDX);
            container->container_material.biological = MAT_COMBINE(v1, v2);
            break;
        case MAT_MAC:
            container->container_state |= BIT(IN_QUEUE_IDX);
            container->container_material.machinery = MAT_COMBINE(v1, v2);
            break;
        default:
            fprintf(stderr, "Unidentified Material: Container in Quarantine\n");
            container->container_state |= BIT(QUARANTINE_IDX);
            break;
    }
  
    // inspect the current container
    inspect(container, mat);

    // add the container to the CargoQueue
    queue->queue[queue->size] = container;
    queue->size += 1;

    free(tc);
}

/**
 * @brief Authorises the ships to load the Container to the 
 * CargoBay.
 * @param CargoQueue queue pointer
 * @param CargoBay bay pointer
 * @return void
 */
void dequeue(CargoQueue *queue, CargoBay *bay, void(*on_dequeue)(CargoBay *bay, Container *container))
{
    if(queue->size == 0)
    {
        fprintf(stderr, "The CargoQueue is empty, there are no ships waiting to deposit a container\n");
        return;
    }
    
    Container *new_container = queue->queue[0];
    queue->queue[0] = NULL;
    queue->size -= 1;
    
    for(int i = 0; i < queue->size; i++)
        if(queue->queue[i+1] != NULL) queue->queue[i] = queue->queue[i+1];

    // callback and load the container to the CargoBay
    on_dequeue(bay, new_container);
}
 

/**
 * @brief Inits the CargoQueue. The CargoQueue simulates the 
 * place in the space station where ships stops before delivering
 * the container their are carrying.
 * @param CargoQueue queue double pointer
 * @return void
 */
void init_cargo_queue(CargoQueue **queue)
{
    CargoQueue *cargo_queue = (CargoQueue *)calloc(1, sizeof(CargoQueue));
    
    if(cargo_queue == NULL)
    {
        fprintf(stderr, "Platform down, cannot load any Container, please come back later\n");
        exit(1);
    }
    
    cargo_queue->capacity = CARGO_QUEUE_CAPACITY;
    cargo_queue->size = 0;

    *queue = cargo_queue;
}

/**
 * @brief Inits the CargoBay. The CargoBay is the place where inspected container 
 * are positioned. The CargoBay has 4 rails where the container are positioned 
 * after the are dequeued from the CargoQueue
 * @param CargoBay bay double pointer
 * @return void
 */
void init_cargo_bay(CargoBay **bay)
{
    CargoBay *cargo_bay = (CargoBay *)calloc(1, sizeof(CargoBay));
    
    if(cargo_bay == NULL)
    {
        fprintf(stderr, "The CargoBay could not be instanciated\n");
        exit(1);
    }
    
    cargo_bay->capacity = CARGO_RAILS_CAPACITY;
    cargo_bay->rails = (Container **)calloc(cargo_bay->capacity, sizeof(Container *));
    if(cargo_bay->rails == NULL)
    {
        fprintf(stderr, "CargoBay's Rails could not be instanciated\n");
        exit(1);
    }

    *bay = cargo_bay;
}

int main(void)
{
    // Init CargoBay
    CargoBay *cargo_bay = NULL;
    init_cargo_bay(&cargo_bay);
    // Init CargoQueue
    CargoQueue *cargo_queue = NULL;
    init_cargo_queue(&cargo_queue);
    // Depositing the container to the CargoQueue
    enqueue(cargo_queue, "BIOEN-0", MAT_BIO, 9, 6);
    enqueue(cargo_queue, "MACHI-0", MAT_MAC, 8, 15);
    enqueue(cargo_queue, "MACHI-1", MAT_MAC, 7, 13);
    enqueue(cargo_queue, "MACHI-2", MAT_MAC, 6, 12);
    enqueue(cargo_queue, "BIOEN-1", MAT_BIO, 12, 4);
    enqueue(cargo_queue, "MACHI-3", MAT_MAC, 4, 8);
    enqueue(cargo_queue, "BIOEN-2", MAT_BIO, 15, 15);
    enqueue(cargo_queue, "BIOEN-3", MAT_BIO, 13, 10);
    enqueue(cargo_queue, "MACHI-4", MAT_MAC, 7, 13);
    enqueue(cargo_queue, "BIOEN-4", MAT_BIO, 1, 0);

    
    // Dequeue Container from CargoQueue and load it to the CargoBay 
    dequeue(cargo_queue, cargo_bay, load);
    dequeue(cargo_queue, cargo_bay, load);
    dequeue(cargo_queue, cargo_bay, load);
    dequeue(cargo_queue, cargo_bay, load);
    dequeue(cargo_queue, cargo_bay, load);
    dequeue(cargo_queue, cargo_bay, load);
    dequeue(cargo_queue, cargo_bay, load);
    dequeue(cargo_queue, cargo_bay, load);
    dequeue(cargo_queue, cargo_bay, load);
    dequeue(cargo_queue, cargo_bay, load);
     
    return 0;
}
