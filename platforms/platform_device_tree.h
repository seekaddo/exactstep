//-----------------------------------------------------------------
//                        ExactStep IAISS
//                             V0.5
//               github.com/ultraembedded/exactstep
//                     Copyright 2014-2019
//                    License: BSD 3-Clause
//-----------------------------------------------------------------
#ifndef __PLATFORM_DEVICE_TREE_H__
#define __PLATFORM_DEVICE_TREE_H__

#include "platform.h"
#include "platform_cpu.h"

#include "device_tree.h"

class platform_device_tree: public platform_cpu
{
public:
    platform_device_tree(const char *misa, const char *filename, console_io *con_io = NULL): platform_cpu(misa, true, 0, 0)
    {
        m_filename = std::string(filename);
        m_console  = con_io;
        m_this_cpu = NULL;
    }

    virtual cpu* get_cpu(void)
    {
        if (!m_this_cpu)
        {
            m_this_cpu = platform_cpu::get_cpu();
            if (!m_this_cpu)
                return NULL;

            device_tree fdt(m_filename.c_str(), m_console);
            if (fdt.load())
            {
                // Attach memories to CPU
                memory_base *mem_next = NULL;
                for (memory_base *mem = fdt.get_memory(); mem != NULL; mem = mem_next)
                {
                    mem_next  = mem->next;
                    mem->next = NULL;

                    m_this_cpu->attach_memory(mem);
                }

                // Attach devices to CPU
                device *dev_next = NULL;
                for (device *dev = fdt.get_device(); dev != NULL; dev = dev_next)
                {
                    dev_next  = dev->device_next;
                    dev->device_next = NULL;

                    m_this_cpu->attach_device(dev);
                }
            }
            else
                fprintf(stderr, "ERROR: Failed to open device tree\n");
        }

        return m_this_cpu;
    }

    cpu * m_this_cpu;

    std::string m_filename;
    console_io *m_console;
};

#endif