/*
 * Copyright (c) 2011-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _DMA_H_
#define _DMA_H_

#include <linux/dmaengine.h>
#include <linux/dma/qcom_bam_dma.h>

/* maximum data transfer block size between BAM and CE */
#define QCE_BAM_BURST_SIZE		64

#define QCE_AUTHIV_REGS_CNT		16
#define QCE_AUTH_BYTECOUNT_REGS_CNT	4
#define QCE_CNTRIV_REGS_CNT		4
#define QCE_BAM_CMD_SGL_SIZE		100
#define QCE_BAM_CMD_ELEMENT_SIZE	100
#define QCE_DMA_DESC_FLAG_BAM_NWD	(0x0004)
#define QCE_DMA_DESC_FLAG_LOCK		(0x0002)
#define QCE_DMA_DESC_FLAG_UNLOCK	(0x0001)
#define QCE_MAX_REG_READ			8


struct qce_result_dump {
	u32 auth_iv[QCE_AUTHIV_REGS_CNT];
	u32 auth_byte_count[QCE_AUTH_BYTECOUNT_REGS_CNT];
	u32 encr_cntr_iv[QCE_CNTRIV_REGS_CNT];
	u32 status;
	u32 status2;
};

#define QCE_IGNORE_BUF_SZ	(2 * QCE_BAM_BURST_SIZE)
#define QCE_RESULT_BUF_SZ	\
		ALIGN(sizeof(struct qce_result_dump), QCE_BAM_BURST_SIZE)

struct qce_dma_data {
	struct dma_chan *txchan;
	struct dma_chan *rxchan;
	struct qce_result_dump *result_buf;
	struct qce_bam_transaction *qce_bam_txn;
	void *ignore_buf;
};

struct qce_desc_info {
	struct dma_async_tx_descriptor *dma_desc;
	enum dma_data_direction dir;
	struct qcom_bam_custom_data bam_desc_data;
};

struct qce_bam_transaction {
	struct bam_cmd_element qce_bam_ce[QCE_BAM_CMD_ELEMENT_SIZE];
	struct qcom_bam_sgl qce_reg_write_sgl[QCE_BAM_CMD_SGL_SIZE];
	struct qcom_bam_sgl qce_reg_read_sgl[QCE_BAM_CMD_SGL_SIZE];
	struct qce_desc_info *qce_desc;
	uint32_t qce_bam_ce_index;
	uint32_t qce_pre_bam_ce_index;
	uint32_t qce_write_sgl_cnt;
	uint32_t qce_read_sgl_cnt;
};

int qce_dma_request(struct device *dev, struct qce_dma_data *dma);
void qce_dma_release(struct qce_dma_data *dma);
int qce_dma_prep_sgs(struct qce_dma_data *dma, struct scatterlist *sg_in,
		     int in_ents, struct scatterlist *sg_out, int out_ents,
		     dma_async_tx_callback cb, void *cb_param);
void qce_dma_issue_pending(struct qce_dma_data *dma);
void qce_dma_issue_cmd_desc_pending(struct qce_dma_data *dma, bool read);
int qce_dma_terminate_all(struct qce_dma_data *dma);
struct scatterlist *
qce_sgtable_add(struct sg_table *sgt, struct scatterlist *sg_add);

#endif /* _DMA_H_ */
