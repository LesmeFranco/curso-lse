/*
 * Copyright 2018, 2020, 2023 NXP
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MEM_MANAGER_H__
#define __MEM_MANAGER_H__

#ifndef SDK_COMPONENT_DEPENDENCY_FSL_COMMON
#define SDK_COMPONENT_DEPENDENCY_FSL_COMMON (1U)
#endif
#if (defined(SDK_COMPONENT_DEPENDENCY_FSL_COMMON) && (SDK_COMPONENT_DEPENDENCY_FSL_COMMON > 0U))
#include "fsl_common.h"
#else
#endif

/*!
 * @addtogroup MemManager
 * @{
 */

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/*!
 * @brief Provide Minimal heap size for application to execute correctly.
 *
 * The application can define a minimal heap size for proper code exection at run time,
 * This will issue a link error if the minimal heap size requirement is not fullfilled (not enough space in RAM)
 * By Default, Minimal heap size is set to 4 bytes (unlikely enough to have application work correctly)
 */
#if !defined(MinimalHeapSize_c)
#define MinimalHeapSize_c (uint32_t)4
#endif

/*!
 * @brief Configures the memory manager light enable.
 */
#ifndef gMemManagerLight
#define gMemManagerLight (1)
#endif

/*!
 * @brief Configures the memory manager trace debug enable.
 */
#ifndef MEM_MANAGER_ENABLE_TRACE
#define MEM_MANAGER_ENABLE_TRACE (0)
#endif

/*
 * @brief Configures the memory manager remove memory buffer.
 */
#ifndef MEM_MANAGER_BUFFER_REMOVE
#define MEM_MANAGER_BUFFER_REMOVE (0)
#endif

/*!
 * @brief Configures the memory manager pre configure.
 */
#ifndef MEM_MANAGER_PRE_CONFIGURE
#define MEM_MANAGER_PRE_CONFIGURE (1)
#endif

#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
#ifndef MEM_POOL_SIZE
#define MEM_POOL_SIZE (32U)
#endif
#ifndef MEM_BLOCK_SIZE
#define MEM_BLOCK_SIZE (12U)
#endif
#else
#ifndef MEM_POOL_SIZE
#define MEM_POOL_SIZE (20U)
#endif
#ifndef MEM_BLOCK_SIZE
#define MEM_BLOCK_SIZE (4U)
#endif
#endif

#define MAX_POOL_ID 3U

/* Debug Macros - stub if not defined */
#ifndef MEM_DBG_LOG
#define MEM_DBG_LOG(...)
#endif

/* Default memory allocator */
#ifndef MEM_BufferAlloc
#define MEM_BufferAlloc(numBytes) MEM_BufferAllocWithId(numBytes, 0)
#endif

#if (defined(MEM_MANAGER_PRE_CONFIGURE) && (MEM_MANAGER_PRE_CONFIGURE > 0U))
/*
 * Defines pools by block size and number of blocks. Must be aligned to 4 bytes.
 * Defines block as  (blockSize ,numberOfBlocks,  id), id must be keep here,
 * even id is 0, will be _block_set_(64, 8, 0) _eol_
 * and _block_set_(64, 8) _eol_\ could not supported
 */
#ifndef PoolsDetails_c
#define PoolsDetails_c _block_set_(64, 8, 0) _eol_ _block_set_(128, 2, 1) _eol_ _block_set_(256, 6, 1) _eol_
#endif /* PoolsDetails_c */

#define MEM_BLOCK_DATA_BUFFER_NONAME_DEFINE(blockSize, numberOfBlocks, id)                                        \
    uint32_t g_poolBuffer##blockSize##_##numberOfBlocks##_##id[(MEM_POOL_SIZE + (numberOfBlocks)*MEM_BLOCK_SIZE + \
                                                                ((numberOfBlocks) * (blockSize)) + 3U) >>         \
                                                               2U];

#define MEM_BLOCK_BUFFER_NONAME_DEFINE(blockSize, numberOfBlocks, id)                   \
    MEM_BLOCK_DATA_BUFFER_NONAME_DEFINE(blockSize, numberOfBlocks, id)                  \
    const static mem_config_t g_poolHeadBuffer##blockSize##_##numberOfBlocks##_##id = { \
        (blockSize), (numberOfBlocks), (id), (0), (uint8_t *)&g_poolBuffer##blockSize##_##numberOfBlocks##_##id[0]}
#define MEM_BLOCK_NONAME_BUFFER(blockSize, numberOfBlocks, id) \
    (uint8_t *)&g_poolHeadBuffer##blockSize##_##numberOfBlocks##_##id
#endif /* MEM_MANAGER_PRE_CONFIGURE */

/*!
 * @brief Defines the memory buffer
 *
 * This macro is used to define the shell memory buffer for memory manager.
 * And then uses the macro MEM_BLOCK_BUFFER to get the memory buffer pointer.
 * The macro should not be used in any function.
 *
 * This is a example,
 * @code
 * MEM_BLOCK_BUFFER_DEFINE(app64, 5, 64,0);
 * MEM_BLOCK_BUFFER_DEFINE(app128, 6, 128,0);
 * MEM_BLOCK_BUFFER_DEFINE(app256, 7, 256,0);
 * @endcode
 *
 * @param name The name string of the memory buffer.
 * @param numberOfBlocks The number Of Blocks.
 * @param blockSize The memory block size.
 * @param id The id Of memory buffer.
 */
#define MEM_BLOCK_DATA_BUFFER_DEFINE(name, numberOfBlocks, blockSize, id) \
    uint32_t                                                              \
        g_poolBuffer##name[(MEM_POOL_SIZE + numberOfBlocks * MEM_BLOCK_SIZE + numberOfBlocks * blockSize + 3U) >> 2U];

#define MEM_BLOCK_BUFFER_DEFINE(name, numberOfBlocks, blockSize, id)  \
    MEM_BLOCK_DATA_BUFFER_DEFINE(name, numberOfBlocks, blockSize, id) \
    mem_config_t g_poolHeadBuffer##name = {(blockSize), (numberOfBlocks), (id), (0), (uint8_t *)&g_poolBuffer##name[0]}

/*!                                                                     \
 * @brief Gets the memory buffer pointer                                 \
 *                                                                       \
 * This macro is used to get the memory buffer pointer. The macro should \
 * not be used before the macro MEM_BLOCK_BUFFER_DEFINE is used.         \
 *                                                                       \
 * @param name The memory name string of the buffer.                     \
 */
#define MEM_BLOCK_BUFFER(name) (uint8_t *)&g_poolHeadBuffer##name

/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/

/**@brief Memory status. */
#if (defined(SDK_COMPONENT_DEPENDENCY_FSL_COMMON) && (SDK_COMPONENT_DEPENDENCY_FSL_COMMON > 0U))
typedef enum _mem_status
{
    kStatus_MemSuccess       = kStatus_Success,                          /* No error occurred */
    kStatus_MemInitError     = MAKE_STATUS(kStatusGroup_MEM_MANAGER, 1), /* Memory initialization error */
    kStatus_MemAllocError    = MAKE_STATUS(kStatusGroup_MEM_MANAGER, 2), /* Memory allocation error */
    kStatus_MemFreeError     = MAKE_STATUS(kStatusGroup_MEM_MANAGER, 3), /* Memory free error */
    kStatus_MemOverFlowError = MAKE_STATUS(kStatusGroup_MEM_MANAGER, 4), /* Over flow has happened... */
    kStatus_MemUnknownError  = MAKE_STATUS(kStatusGroup_MEM_MANAGER, 5), /* something bad has happened... */
} mem_status_t;
#else
typedef enum _mem_status
{
    kStatus_MemSuccess       = 0, /* No error occurred */
    kStatus_MemInitError     = 1, /* Memory initialization error */
    kStatus_MemAllocError    = 2, /* Memory allocation error */
    kStatus_MemFreeError     = 3, /* Memory free error */
    kStatus_MemOverFlowError = 4, /* Over flow error */
    kStatus_MemUnknownError  = 5, /* something bad has happened... */
} mem_status_t;

#endif

/**@brief Memory user config. */
typedef struct _mem_config
{
    uint16_t blockSize;      /*< The memory block size. */
    uint16_t numberOfBlocks; /*< The number Of Blocks. */
    uint16_t poolId;         /*< The pool id Of Blocks. */
    uint16_t reserved;       /*< reserved. */
    uint8_t *pbuffer;        /*< buffer. */
} mem_config_t;

#if defined(gFSCI_MemAllocTest_Enabled_d) && (gFSCI_MemAllocTest_Enabled_d)
/**@brief Memory status. */
typedef enum mem_alloc_test_status
{
    kStatus_AllocSuccess = kStatus_Success, /* Allow buffer to be allocated */
    kStatus_AllocBlock   = kStatus_Busy,    /* Block buffer to be allocated */
} mem_alloc_test_status_t;
#endif

#ifdef MEM_STATISTICS
#define MML_INTERNAL_STRUCT_SZ (2 * sizeof(uint32_t) + 48)
#else
#define MML_INTERNAL_STRUCT_SZ (2 * sizeof(uint32_t))
#endif

#define AREA_FLAGS_POOL_NOT_SHARED (1u << 0)
#define AREA_FLAGS_VALID_MASK      (AREA_FLAGS_POOL_NOT_SHARED)
#define AREA_FLAGS_RFFU            ~(AREA_FLAGS_VALID_MASK)

/**@brief Memory user config. */
typedef struct _mem_area_cfg_s memAreaCfg_t;
struct _mem_area_cfg_s
{
    memAreaCfg_t *next;     /*< Next registered RAM area descriptor. */
    void *start_address;    /*< Start address of RAM area. */
    void *end_address;      /*< Size of registered RAM area. */
    uint16_t flags;         /*< BIT(0) AREA_FLAGS_POOL_NOT_SHARED means not member of default pool, other bits RFFU */
    uint16_t reserved;      /*< 16 bit padding */
    uint32_t low_watermark; /*< lowest level of number of free bytes */
    uint8_t internal_ctx[MML_INTERNAL_STRUCT_SZ]; /* Placeholder for internal allocator data */
};

/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/
/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */
#if (defined(MEM_MANAGER_PRE_CONFIGURE) && (MEM_MANAGER_PRE_CONFIGURE > 0U))
/*!
 * @brief  Initialises the Memory Manager.
 *
 */
mem_status_t MEM_Init(void);

#endif

#if !defined(gMemManagerLight) || (gMemManagerLight == 0)
/*!
 * @brief Add memory buffer to memory manager buffer list.
 *
 * @note This API should be called when need add memory buffer to memory manager buffer list. First use
 * MEM_BLOCK_BUFFER_DEFINE to
 *        define memory buffer, then call MEM_AddBuffer function with MEM_BLOCK_BUFFER Macro as the input parameter.
 *  @code
 * MEM_BLOCK_BUFFER_DEFINE(app64, 5, 64,0);
 * MEM_BLOCK_BUFFER_DEFINE(app128, 6, 128,0);
 * MEM_BLOCK_BUFFER_DEFINE(app256, 7, 256,0);
 *
 * MEM_AddBuffer(MEM_BLOCK_BUFFER(app64));
 * MEM_AddBuffer(MEM_BLOCK_BUFFER(app128));
 * MEM_AddBuffer(MEM_BLOCK_BUFFER(app256));
 * @endcode
 *
 * @param buffer                     Pointer the memory pool buffer, use MEM_BLOCK_BUFFER Macro as the input parameter.
 *
 * @retval kStatus_MemSuccess        Memory manager add buffer succeed.
 * @retval kStatus_MemUnknownError   Memory manager add buffer error occurred.
 */
mem_status_t MEM_AddBuffer(const uint8_t *buffer);
#endif /* gMemManagerLight */

#if !defined(gMemManagerLight) || (gMemManagerLight == 0)
#if (defined(MEM_MANAGER_BUFFER_REMOVE) && (MEM_MANAGER_BUFFER_REMOVE > 0U))
/*!
 * @brief Remove memory buffer from memory manager buffer list.
 *
 * @note This API should be called when need remove memory buffer from memory manager buffer list. Use MEM_BLOCK_BUFFER
 * Macro as the input parameter.
 *
 * @param buffer                     Pointer the memory pool buffer, use MEM_BLOCK_BUFFER Macro as the input parameter.
 *
 * @retval kStatus_MemSuccess        Memory manager remove buffer succeed.
 * @retval kStatus_MemUnknownError    Memory manager remove buffer error occurred.
 */
mem_status_t MEM_RemoveBuffer(uint8_t *buffer);
#endif /* MEM_MANAGER_BUFFER_REMOVE */
#endif /* gMemManagerLight */
/*!
 * @brief Allocate a block from the memory pools. The function uses the
 *        numBytes argument to look up a pool with adequate block sizes.
 *
 * @param numBytes           The number of bytes will be allocated.
 * @param poolId             The ID of the pool where to search for a free buffer.
 * @retval Memory buffer address when allocate success, NULL when allocate fail.
 */
void *MEM_BufferAllocWithId(uint32_t numBytes, uint8_t poolId);

/*!
 * @brief Memory buffer free .
 *
 * @param buffer                     The memory buffer address will be free.
 * @retval kStatus_MemSuccess        Memory free succeed.
 * @retval kStatus_MemFreeError      Memory free error occurred.
 */
mem_status_t MEM_BufferFree(void *buffer);

/*!
 * @brief Returns the size of a given buffer.
 *
 * @param buffer  The memory buffer address will be get size.
 * @retval The size of a given buffer.
 */
uint16_t MEM_BufferGetSize(void *buffer);

/*!
 * @brief Frees all allocated blocks by selected source and in selected pool.
 *
 * @param poolId                     Selected pool Id (4 LSBs of poolId parameter) and selected
 *                                   source Id (4 MSBs of poolId parameter).
 * @retval kStatus_MemSuccess        Memory free succeed.
 * @retval kStatus_MemFreeError      Memory free error occurred.
 */
mem_status_t MEM_BufferFreeAllWithId(uint8_t poolId);

/*!
 * @brief Memory buffer realloc.
 *
 * @param buffer                     The memory buffer address will be reallocated.
 * @param new_size                   The number of bytes will be reallocated
 * @retval kStatus_MemSuccess        Memory free succeed.
 * @retval kStatus_MemFreeError      Memory free error occurred.
 */
void *MEM_BufferRealloc(void *buffer, uint32_t new_size);

/*!
 * @brief Get the address after the last allocated block if MemManagerLight is used.
 *
 * @retval UpperLimit  Return the address after the last allocated block if MemManagerLight is used.
 * @retval 0           Return 0 in case of the legacy MemManager.
 */
uint32_t MEM_GetHeapUpperLimit(void);

#if defined(gMemManagerLight) && (gMemManagerLight > 0)
/*!
 * @brief Get the address after the last allocated block in area defined by id.
 *
 * @param[in] id       0 means memHeap, other values depend on number of registered areas
 *
 * @retval UpperLimit  Return the address after the last allocated block if MemManagerLight is used.
 * @retval 0           Return 0 in case of the legacy MemManager.
 */
uint32_t MEM_GetHeapUpperLimitByAreaId(uint8_t area_id);
#endif

/*!
 * @brief Get the free space low watermark.
 *
 * @retval FreeHeapSize  Return the heap space low water mark free if MemManagerLight is used.
 * @retval 0             Return 0 in case of the legacy MemManager.
 */
uint32_t MEM_GetFreeHeapSizeLowWaterMark(void);

/*!
 * @brief Get the free space low watermark.
 *
 * @param area_id       Selected area Id
 *
 * @retval             Return the heap space low water mark free if MemManagerLight is used.
 * @retval 0           Return 0 in case of the legacy MemManager.
 */
uint32_t MEM_GetFreeHeapSizeLowWaterMarkByAreaId(uint8_t area_id);

/*!
 * @brief Reset the free space low watermark.
 *
 * @retval FreeHeapSize  Return the heap space low water mark free at the time it was reset
 *                       if MemManagerLight is used.
 * @retval 0             Return 0 in case of the legacy MemManager.
 */
uint32_t MEM_ResetFreeHeapSizeLowWaterMark(void);

/*!
 * @brief Reset the free space low watermark.
 *
 * @param area_id       Selected area Id
 *
 * @retval FreeHeapSize  Return the heap space low water mark free at the time it was reset
 *                       if MemManagerLight is used.
 * @retval 0             Return 0 in case of the legacy MemManager.
 */
uint32_t MEM_ResetFreeHeapSizeLowWaterMarkByAreaId(uint8_t area_id);

/*!
 * @brief Get the free space in the heap for a area id.
 *
 * @param area_id        area_id whose available size is requested (0 means generic pool)
 *
 * @retval FreeHeapSize  Return the free space in the heap if MemManagerLight is used.
 * @retval 0             Return 0 in case of the legacy MemManager.
 */
uint32_t MEM_GetFreeHeapSizeByAreaId(uint8_t area_id);

/*!
 * @brief Get the free space in the heap.
 *
 * @retval FreeHeapSize  Return the free space in the heap if MemManagerLight is used.
 * @retval 0             Return 0 in case of the legacy MemManager.
 */
uint32_t MEM_GetFreeHeapSize(void);

#if defined(gMemManagerLight) && (gMemManagerLight > 0)
/*!
 * @brief Selective RAM bank reinit after low power, based on a requested address range
 *        Useful for ECC RAM banks
 *        Defined as weak and empty in fsl_component_mem_manager_light.c to be overloaded by user
 *
 * @param[in] startAddress Start address of the requested range
 * @param[in] endAddress End address of the requested range
 */
void MEM_ReinitRamBank(uint32_t startAddress, uint32_t endAddress);
#endif /* gMemManagerLight */

#if !defined(gMemManagerLight) || (gMemManagerLight == 0)
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
/*!
 * @brief Function to print statistics related to memory blocks managed by memory manager. Like bellow:
 * allocationFailures: 241  freeFailures:0
 * POOL: ID 0  status:
 * numBlocks allocatedBlocks    allocatedBlocksPeak  poolFragmentWaste poolFragmentWastePeak poolFragmentMinWaste
 * poolTotalFragmentWaste
 *     5            5                 5                  59                  63                       59 305
 * Currently pool meory block allocate status:
 * Block 0 Allocated    bytes: 1
 * Block 1 Allocated    bytes: 2
 * Block 2 Allocated    bytes: 3
 * Block 3 Allocated    bytes: 4
 * Block 4 Allocated    bytes: 5
 *
 * @details This API prints information with respects to each pool and block, including Allocated size,
 *          total block count, number of blocks in use at the time of printing, The API is intended to
 *          help developers tune the block sizes to make optimal use of memory for the application.
 *
 * @note This API should be disable by configure MEM_MANAGER_ENABLE_TRACE to 0
 *
 */
void MEM_Trace(void);

#endif /* MEM_MANAGER_ENABLE_TRACE */
#endif /* gMemManagerLight */

#if defined(gMemManagerLight) && (gMemManagerLight == 1)
void *MEM_CallocAlt(size_t len, size_t val);
#endif /*gMemManagerLight == 1*/

#if defined(gMemManagerLight) && (gMemManagerLight > 0)
/*!
 * @brief Function to register additional areas to allocate memory from.
 *
 * @param[in]  area_desc memAreaCfg_t structure defining start address and end address of area.
 *             This atructure may not be in rodata becasue the next field and internal private
 *             context are reserved in this structure. If NULL defines the default memHeap area.
 * @param[out] area_id pointer to return id of area. Required if allocation from specific pool
 *             is required.
 * @param[in]  flags BIT(0) means that allocations can be performed in pool only explicitly and
 *             it is not a member of the default pool (id 0). Invalid for initial registration call.
 * @return   kStatus_MemSuccess if success,  kStatus_MemInitError otherwise.
 *
 */
mem_status_t MEM_RegisterExtendedArea(memAreaCfg_t *area_desc, uint8_t *p_area_id, uint16_t flags);

/*!
 * @brief Function to unregister an extended area
 *
 * @param[in]  area_id must be different from 0 (main heap).
 *
 * @return   kStatus_MemSuccess if success,
 *           kStatus_MemFreeError if area_id is 0 or area not found or still has buffers in use.
 *
 */
mem_status_t MEM_UnRegisterExtendedArea(uint8_t area_id);

#endif

/*!
* \brief     This function to check for buffer overflow when copying multiple bytes
*
* \param[in] p    - pointer to destination.
* \param[in] size - number of bytes to copy
*
 * @return   kStatus_MemOverFlowError if buffer overflow.
 *
 */
mem_status_t MEM_BufferCheck(void *buffer, uint32_t size);

#if defined(__cplusplus)
}
#endif
/*! @}*/
#endif /* #ifndef __MEM_MANAGER_H__ */
