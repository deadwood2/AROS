/*
    Copyright (C) 2020-2023, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>

#include <asm/io.h>
#include <hidd/pci.h>
#include <interface/Hidd_PCIDriver.h>

#include <string.h>

#if defined(__AROSEXEC_SMP__)
#include <proto/kernel.h>
#endif

#include "nvme_debug.h"
#include "nvme_intern.h"
#include "nvme_hw.h"

/*
    Admin Queue Support Functions
*/

void nvme_complete_adminevent(struct nvme_queue *nvmeq, struct nvme_completion *cqe)
{
    struct completionevent_handler *handler;

    D(bug("[NVME:ADMINQ] %s(0x%p)\n", __func__, cqe);)

    if ((handler = nvmeq->cehandlers[cqe->command_id]) != NULL)
    {
        D(bug ("[NVME:ADMINQ] %s: cehandler @ 0x%p\n", __func__, handler);)
        handler->ceh_Result = AROS_LE2LONG(cqe->result);
        handler->ceh_Status = AROS_LE2WORD(cqe->status) >> 1;
        nvmeq->cehandlers[cqe->command_id] = NULL;
        D(bug ("[NVME:ADMINQ] %s: Signaling 0x%p (%08x)\n", __func__, handler->ceh_Task, handler->ceh_SigSet);)
        Signal(handler->ceh_Task, handler->ceh_SigSet);
    }
}

int nvme_submit_admincmd(device_t dev, struct nvme_command *cmd, struct completionevent_handler *handler)
{
    int retval;

    D(bug("[NVME:ADMINQ] %s(0x%p, 0x%p)\n", __func__, dev, cmd);)

    cmd->common.op.command_id = nvme_alloc_cmdid(dev->dev_Queues[0]);

    dev->dev_Queues[0]->cehooks[cmd->common.op.command_id] = nvme_complete_adminevent;
    dev->dev_Queues[0]->cehandlers[cmd->common.op.command_id] = handler;

    if (handler)
    {
        /* clear the signal first */
        SetSignal(0, handler->ceh_SigSet);
    }

    retval = nvme_submit_cmd(dev->dev_Queues[0], cmd);

    return retval;
}
