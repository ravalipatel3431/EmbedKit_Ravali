#include <stdio.h>      // Standard I/O functions
#include <stdint.h>     // Fixed-width integer types
#include <stddef.h>     // NULL definition

/* Ring Buffer Configuration */
#define BUFFER_SIZE    (8U)   // Buffer size (must be power of 2)
#define READ_COUNT     (3U)   // Number of bytes to read initially

/* Return Status Codes */
#define RB_OK          (0)    // Operation successful
#define RB_ERROR       (-1)   // Operation failed

/* Ring Buffer Structure */
typedef struct
{
    uint8_t buffer[BUFFER_SIZE];  // Storage array
    uint8_t head;                 // Next write position
    uint8_t tail;                 // Next read position
    uint8_t count;                // Number of bytes currently stored
} RingBuffer;

/* Initialize Ring Buffer */
void RingBuffer_Init(RingBuffer *rb)
{
    if (rb == NULL)
    {
        return;
    }

    rb->head = 0U;     // Reset write index
    rb->tail = 0U;     // Reset read index
    rb->count = 0U;    // Buffer initially empty
}

/* Check whether buffer is full */
int RingBuffer_IsFull(const RingBuffer *rb)
{
    if (rb == NULL)
    {
        return 0;
    }

    return (rb->count == BUFFER_SIZE);
}

/* Check whether buffer is empty */
int RingBuffer_IsEmpty(const RingBuffer *rb)
{
    if (rb == NULL)
    {
        return 1;
    }

    return (rb->count == 0U);
}

/* Return current number of stored bytes */
uint8_t RingBuffer_Count(const RingBuffer *rb)
{
    if (rb == NULL)
    {
        return 0U;
    }

    return rb->count;
}

/* Write one byte into Ring Buffer */
int RingBuffer_Write(RingBuffer *rb, uint8_t data)
{
    if (rb == NULL)
    {
        return RB_ERROR;
    }

    /* Reject write when buffer is full */
    if (RingBuffer_IsFull(rb))
    {
        return RB_ERROR;
    }

    /* Store data at current head position */
    rb->buffer[rb->head] = data;

    /*
     * Move head to next position.
     * Using bitwise AND instead of modulo (%).
     * Faster on MCUs without a hardware divider because
     * it avoids a division operation.
     * Works only when BUFFER_SIZE is a power of 2.
     */
    rb->head = (rb->head + 1U) & (BUFFER_SIZE - 1U);

    /* Increment stored byte count */
    rb->count++;

    return RB_OK;
}

/* Read one byte from Ring Buffer */
int RingBuffer_Read(RingBuffer *rb, uint8_t *data)
{
    if ((rb == NULL) || (data == NULL))
    {
        return RB_ERROR;
    }

    /* Reject read when buffer is empty */
    if (RingBuffer_IsEmpty(rb))
    {
        return RB_ERROR;
    }

    /* Read data from current tail position */
    *data = rb->buffer[rb->tail];

    /*
     * Move tail to next position.
     * Using bitwise AND instead of modulo (%).
     * Faster on MCUs without a hardware divider because
     * it avoids a division operation.
     * Works only when BUFFER_SIZE is a power of 2.
     */
    rb->tail = (rb->tail + 1U) & (BUFFER_SIZE - 1U);

    /* Decrement stored byte count */
    rb->count--;

    return RB_OK;
}

int main(void)
{
    /* Create Ring Buffer instance */
    RingBuffer rb;

    /* Variable used to store data read from buffer */
    uint8_t data;

    /* Initial data used to fill the buffer */
    const uint8_t write_data[BUFFER_SIZE] =
    {
        0x41U, 0x42U, 0x43U, 0x44U,
        0x45U, 0x46U, 0x47U, 0x48U
    };

    /* New data used to verify wrap-around functionality */
    const uint8_t new_data[READ_COUNT] =
    {
        0x49U, 0x4AU, 0x4BU
    };

    /* Initialize Ring Buffer before use */
    RingBuffer_Init(&rb);

    /***** STEP 1 *****/
    /* Fill the buffer with 8 bytes and verify full condition */
    printf("***** STEP 1 *****\n");

    for (uint8_t i = 0U; i < BUFFER_SIZE; i++)
    {
        if (RingBuffer_Write(&rb, write_data[i]) == RB_OK)
        {
            printf("[WRITE] 0x%02X -> OK (count=%u)",
                   write_data[i],
                   RingBuffer_Count(&rb));

            /* Check if buffer reached maximum capacity */
            if (RingBuffer_IsFull(&rb))
            {
                printf(" FULL");
            }

            printf("\n");
        }
    }

    /* Confirm buffer is completely full */
    printf("Buffer Full = %s\n",
           RingBuffer_IsFull(&rb) ? "YES" : "NO");

    /***** STEP 2 *****/
    /* Attempt to write when buffer is already full */
    printf("\n***** STEP 2 *****\n");

    if (RingBuffer_Write(&rb, 0x99U) == RB_ERROR)
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

    /***** STEP 3 *****/
    /* Read first three bytes from the buffer */
    printf("\n***** STEP 3 *****\n");

    for (uint8_t i = 0U; i < READ_COUNT; i++)
    {
        if (RingBuffer_Read(&rb, &data) == RB_OK)
        {
            printf("[READ] -> 0x%02X (count=%u)\n",
                   data,
                   RingBuffer_Count(&rb));
        }
    }

    /***** STEP 4 *****/
    /* Reuse freed locations by writing three new bytes */
    printf("\n***** STEP 4 *****\n");

    for (uint8_t i = 0U; i < READ_COUNT; i++)
    {
        if (RingBuffer_Write(&rb, new_data[i]) == RB_OK)
        {
            printf("[WRITE] 0x%02X -> OK (count=%u)\n",
                   new_data[i],
                   RingBuffer_Count(&rb));
        }
    }

    /* Verify buffer becomes full again */
    printf("Buffer Full = %s\n",
           RingBuffer_IsFull(&rb) ? "YES" : "NO");

    /***** STEP 5 *****/
    /* Read all remaining bytes until buffer becomes empty */
    printf("\n***** STEP 5 *****\n");

    while (!RingBuffer_IsEmpty(&rb))
    {
        if (RingBuffer_Read(&rb, &data) == RB_OK)
        {
            printf("[READ] -> 0x%02X (count=%u)\n",
                   data,
                   RingBuffer_Count(&rb));
        }
    }

    /* Confirm buffer is empty */
    printf("Buffer Empty = %s\n",
           RingBuffer_IsEmpty(&rb) ? "YES" : "NO");

    /***** STEP 6 *****/
    /* Attempt to read from an empty buffer */
    printf("\n***** STEP 6 *****\n");

    if (RingBuffer_Read(&rb, &data) == RB_ERROR)
    {
        printf("[READ] (empty) -> FAIL (buffer empty)\n");
    }

    return 0;
}
