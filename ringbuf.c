#include <stdio.h>      // printf()
#include <stdint.h>     // uint8_t data types

/* Must be a power of 2 for '& (BUFFER_SIZE - 1)' wrap-around */
#define BUFFER_SIZE 8   // Fixed ring buffer size

/* Ring Buffer structure */
typedef struct
{
    uint8_t buffer[BUFFER_SIZE]; // Storage array
    uint8_t head;               // Next write position
    uint8_t tail;               // Next read position
    uint8_t count;              // Number of bytes currently stored
} RingBuffer;

/* Initialize the ring buffer to an empty state */
void RingBuffer_Init(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

/* Returns 1 if buffer is full, otherwise 0 */
int RingBuffer_IsFull(RingBuffer *rb)
{
    return (rb->count == BUFFER_SIZE);
}

/* Returns 1 if buffer is empty, otherwise 0 */
int RingBuffer_IsEmpty(RingBuffer *rb)
{
    return (rb->count == 0);
}

/* Returns current number of bytes stored */
uint8_t RingBuffer_Count(RingBuffer *rb)
{
    return rb->count;
}

/* Write one byte into the buffer */
int RingBuffer_Write(RingBuffer *rb, uint8_t data)
{
    /* Reject write if buffer is full */
    if (RingBuffer_IsFull(rb))
    {
        return -1;
    }

    /* Store data at current head position */
    rb->buffer[rb->head] = data;

    /*
    * Using bitwise AND instead of modulo (%).
    * This is faster on MCUs without a hardware divider.
    * It works only when BUFFER_SIZE is a power of 2.
    */
    rb->head = (rb->head + 1) & (BUFFER_SIZE - 1);

    /* Increase stored byte count */
    rb->count++;

    return 0;
}

/* Read one byte from the buffer */
int RingBuffer_Read(RingBuffer *rb, uint8_t *data)
{
    /* Reject read if buffer is empty */
    if (RingBuffer_IsEmpty(rb))
    {
        return -1;
    }

    /* Read data from current tail position */
    *data = rb->buffer[rb->tail];

    /*
     * Using bitwise AND instead of modulo (%).
     * This is faster on MCUs without a hardware divider.
     * It works only when BUFFER_SIZE is a power of 2.
     */
    rb->tail = (rb->tail + 1) & (BUFFER_SIZE - 1);

    /* Decrease stored byte count */
    rb->count--;

    return 0;
}

int main()
{
    /* Create ring buffer instance */
    RingBuffer rb;

    /* Variable used to store read data */
    uint8_t data;

    /* Initial data to fill the buffer */
    uint8_t write_data[] =
    {
        0x41, 0x42, 0x43, 0x44,
        0x45, 0x46, 0x47, 0x48
    };

    /* Initialize ring buffer */
    RingBuffer_Init(&rb);

    /************* STEP 1 *************/
    /* Write 8 bytes into the buffer */
    printf("***** STEP 1 *****\n");

    for (int i = 0; i < 8; i++)
    {
        if (RingBuffer_Write(&rb, write_data[i]) == 0)
        {
            printf("[WRITE] 0x%02X -> OK (count=%d)",
                   write_data[i],
                   RingBuffer_Count(&rb));

            /* Check if buffer becomes full */
            if (RingBuffer_IsFull(&rb))
            {
                printf(" FULL");
            }

            printf("\n");
        }
    }

    /************* STEP 2 *************/
    /* Try writing when buffer is already full */
    printf("\n***** STEP 2 *****\n");

    if (RingBuffer_Write(&rb, 0x99) != 0)
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }
    
    /************* STEP 3 *************/
    /* Read first 3 bytes from buffer */
    printf("\n***** STEP 3 *****\n");

    for (int i = 0; i < 3; i++)
    {
        if (RingBuffer_Read(&rb, &data) == 0)
        {
            printf("[READ] -> 0x%02X (count=%d)\n",
                   data,
                   RingBuffer_Count(&rb));
        }
    }

    /************* STEP 4 *************/
    /* Reuse freed slots by writing 3 new bytes */
    printf("\n***** STEP 4 *****\n");

    uint8_t new_data[] = {0x49, 0x4A, 0x4B};

    for (int i = 0; i < 3; i++)
    {
        if (RingBuffer_Write(&rb, new_data[i]) == 0)
        {
            printf("[WRITE] 0x%02X -> OK (count=%d)\n",
                   new_data[i],
                   RingBuffer_Count(&rb));
        }
    }

    /************* STEP 5 *************/
    /* Read all remaining bytes until buffer becomes empty */
    printf("\n***** STEP 5 *****\n");

    while (!RingBuffer_IsEmpty(&rb))
    {
        if (RingBuffer_Read(&rb, &data) == 0)
        {
            printf("[READ] -> 0x%02X (count=%d)\n",
                   data,
                   RingBuffer_Count(&rb));
        }
    }

    /* Confirm buffer is empty */
    printf("Buffer Empty = YES\n");

    /************* STEP 6 *************/
    /* Attempt to read from an empty buffer */
    printf("\n***** STEP 6 *****\n");

    if (RingBuffer_Read(&rb, &data) != 0)
    {
        printf("[READ] (empty) -> FAIL (buffer empty)\n");
    }

    return 0;
}
