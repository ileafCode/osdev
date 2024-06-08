#include <stdint.h>
#include "cstr.h"
#include "stdio/stdio.h"

namespace PCI
{
    const char *dev_classes[] = {
        "Unclassified Device",                // 0x00
        "Mass Storage Controller",            // 0x01
        "Networking Controller",              // 0x02
        "Display Controller",                 // 0x03
        "Multimedia Controller",              // 0x04
        "Memory Controller",                  // 0x05
        "Bridge",                             // 0x06
        "Basic Communication Controller",     // 0x07
        "Base System Peripheral",             // 0x08
        "Input Device Controller",            // 0x09
        "Docking Station",                    // 0x0A
        "Processor",                          // 0x0B
        "Serial Bus Controller",              // 0x0C
        "Wireless Controller",                // 0x0D
        "Intelligent Controller",             // 0x0E
        "Sattelite Communication Controller", // 0x0F
        "Encryption Controller",              // 0x10
        "Signal Processor Controller",        // 0x11
        "Processing Accelerator",             // 0x12
        "Non-Essential Instrumentation",      // 0x13
    };

    const char *GetDeviceClassName(uint64_t dev)
    {
        if (dev == 0x40)
            return "Coprocessor";
        if (dev > 0x13)
            return "PCI_UNKNOWN";
        return dev_classes[dev];
    }

    const char *GetVendorName(uint16_t vendor)
    {
        switch (vendor)
        {
        case 0x8086:
            return "Intel Corporation";
        case 0x1022:
            return "Advanced Micro Devices (AMD)";
        case 0x10DE:
            return "NVIDIA Corporation";
        case 0x14E4:
            return "Broadcom Inc.";
        case 0x10EC:
            return "Realtek Semiconductor Corp.";
        case 0x168C:
            return "Qualcomm Atheros";
        case 0x104C:
            return "Texas Instruments";
        case 0x1106:
            return "VIA Technologies, Inc.";
        case 0x1B4B:
            return "Marvell Technology Group Ltd.";
        case 0x1043:
            return "ASUSTeK Computer Inc.";
        case 0x1028:
            return "Dell Inc.";
        case 0x103C:
            return "Hewlett Packard Enterprise (HPE)";
        case 0x17AA:
            return "Lenovo Group Limited";
        case 0x144D:
            return "Samsung Electronics Co., Ltd.";
        case 0x1414:
            return "Microsoft Corporation";
        case 0x106B:
            return "Apple Inc.";
        case 0x104D:
            return "Sony Corporation";
        case 0x1B74:
            return "LG Electronics";
        case 0x1179:
            return "Toshiba Corporation";
        case 0x10CF:
            return "Fujitsu Limited";
        case 0x106C:
            return "Panasonic Corporation";
        case 0x1033:
            return "NEC Corporation";
        case 0x1014:
            return "IBM Corporation";
        case 0x1025:
            return "Acer Inc.";
        case 0x1458:
            return "Gigabyte Technology Co., Ltd.";

        default:
            return "Unknown/Undefined Vendor";
        }
    }

    // === DEVICE === //

    const char *intel_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x29C0:
            return "Express DRAM Controller";
        case 0x2918:
            return "LPC Interface Controller";
        case 0x2922:
            return "6 Port SATA Controller [AHCI]";
        case 0x2930:
            return "SMBus Controller";
        case 0x0F41:
            return "Ethernet Connection (2) I219-V";
        case 0x9C03:
            return "Wireless-AC 9560";
        case 0x9D71:
            return "Thunderbolt 3 Controller";
        case 0x9DF0:
            return "Serial IO I2C Host Controller";
        case 0xA2AF:
            return "Ethernet Connection (7) I219-LM";
        case 0x9DEE:
            return "Serial IO I2C Host Controller";
        case 0x8C3A:
            return "HD Graphics 5500";
        case 0x3EA0:
            return "Xeon E3-1200 v5/E3-1500 v5/6th Gen Core Processor Host Bridge/DRAM Registers";
        case 0x6F54:
            return "Thunderbolt Controller";
        case 0x9DE8:
            return "Serial IO I2C Host Controller";
        case 0x9DE6:
            return "Serial IO I2C Host Controller";
        case 0x8C22:
            return "HD Graphics 2000";
        case 0x2415:
            return "AC97 Audio";

        default:
            return "Unknown/Undefined Intel Device";
        }
    }

    const char *amd_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x1537:
            return "Radeon HD 6520G";
        case 0x1309:
            return "SATA Controller [AHCI mode]";
        case 0x4391:
            return "RTL8139 Ethernet";
        case 0x2000:
            return "PCnet Ethernet Controller (PCnet32 LANCE)";
        case 0x7801:
            return "USB 3.0 Host Controller";
        case 0x43A3:
            return "R9 280X";
        case 0x1470:
            return "AMD K8 [Athlon64/Opteron] HyperTransport Technology Configuration";
        case 0x1705:
            return "RV635/M86 [Mobility Radeon HD 3650/3730]";
        case 0x1636:
            return "AMD Radeon HD 7340 Graphics";
        case 0x15DD:
            return "SB7x0/SB8x0/SB9x0 USB EHCI Controller";
        case 0x1578:
            return "AMD Radeon R7 Graphics";
        case 0x1650:
            return "AMD Radeon HD 8400 / R3 Series";
        case 0x43B1:
            return "R9 290X";
        case 0x43BB:
            return "Tahiti XT HDMI Audio [Radeon HD 7970 Series]";
        case 0x1469:
            return "AMD K8 [Athlon64/Opteron] DRAM Controller";
        case 0x43C0:
            return "AMD Radeon RX 5600/5700/XT";
        case 0x43D5:
            return "Navi 10 [Radeon RX 5600 OEM/5600 XT / 5700/5700 XT]";
        case 0x15D8:
            return "AMD FCH Azalia Controller";
        case 0x43DF:
            return "Navi 14 [Radeon RX 5500/5500M / Pro 5500M]";
        case 0x15D9:
            return "AMD FCH SMBus Controller";
        case 0x1403:
            return "AMD-8131 PCI-X Bridge";

        default:
            return "Unknown/Undefined AMD Device";
        }
    }

    const char *nvidia_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x10DE:
            return "GeForce GTX 1080";
        case 0x0F02:
            return "Quadro P6000";
        case 0x1B81:
            return "GeForce GTX 1060";
        case 0x13C2:
            return "Tesla K40m";
        case 0x0F04:
            return "Quadro P5000";
        case 0x1C02:
            return "GeForce GTX 1050 Ti";
        case 0x1B80:
            return "GeForce GTX 1070";
        case 0x0FC0:
            return "GRID M60-1Q";
        case 0x1B06:
            return "GeForce GTX 1080 Ti";
        case 0x1DB6:
            return "GeForce RTX 2080 Ti";
        case 0x0FE9:
            return "Quadro P2000";
        case 0x1C30:
            return "GeForce GT 1030";
        case 0x1F82:
            return "GeForce GTX 1650 SUPER";
        case 0x0FF3:
            return "Quadro P620";
        case 0x1C82:
            return "GeForce GTX 1050";
        case 0x1F02:
            return "GeForce GTX 1660 Ti";
        case 0x1BA1:
            return "GeForce GTX 1050 Ti Mobile";
        case 0x1F08:
            return "GeForce GTX 1660";
        case 0x1C03:
            return "GeForce GTX 1650";
        case 0x1DB5:
            return "GeForce RTX 2080";
        case 0x0FE8:
            return "Quadro P1000";
        case 0x1BB1:
            return "GeForce GTX 1060 6GB";
        case 0x1FB8:
            return "GeForce RTX 3060";
        case 0x1F14:
            return "GeForce GTX 1660 SUPER";
        case 0x1F06:
            return "GeForce GTX 1660 SUPER Mobile";
        case 0x1B38:
            return "GeForce MX150";
        case 0x1C8C:
            return "GeForce GTX 1050 Mobile";
        case 0x1BA0:
            return "GeForce GTX 1050 Mobile";
        case 0x1D10:
            return "GeForce RTX 2070 SUPER";

        default:
            return "Unknown/Undefined NVIDIA Device";
        }
    }

    const char *broadcom_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x16A7:
            return "BCM43142A0 Bluetooth";
        case 0x16BE:
            return "NetXtreme BCM5720 Gigabit Ethernet PCIe";
        case 0x1657:
            return "NetXtreme II BCM5709 Gigabit Ethernet";
        case 0x16B5:
            return "NetXtreme II BCM57810 10 Gigabit Ethernet";
        case 0x16A3:
            return "NetXtreme II BCM57840 10/25/40 Gigabit Ethernet";
        case 0x1653:
            return "NetXtreme BCM5722 Gigabit Ethernet PCIe";
        case 0x1654:
            return "NetLink BCM57780 Gigabit Ethernet PCIe";
        case 0x16A8:
            return "NetLink BCM57788 Gigabit Ethernet PCIe";
        case 0x16BD:
            return "NetXtreme BCM5721 Gigabit Ethernet PCIe";
        case 0x16D1:
            return "BCM4313 802.11bgn Wireless Network Adapter";

        default:
            return "Unknown/Undefined Broadcom Device";
        }
    }
    const char *realtek_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x8168:
            return "RTL8111/8168/8411 Gigabit Ethernet Controller";
        case 0x8179:
            return "RTL8188E Wireless Network Adapter";
        case 0x8723:
            return "RTL8723BE PCIe Wireless Network Adapter";
        case 0x8192:
            return "RTL8192EE PCIe Wireless Network Adapter";
        case 0x10EC:
            return "RTL810xE PCI Express Fast Ethernet Controller";
        case 0x8172:
            return "RTL8191SEvB Wireless LAN Controller";
        case 0x8136:
            return "RTL8101/2/6E PCI Express Fast/Gigabit Ethernet Controller";
        case 0x8169:
            return "RTL8169/8110 Family Gigabit Ethernet NIC";
        case 0xB723:
            return "RTL8723DE PCIe Wireless Network Adapter";
        case 0x8161:
            return "RTL8168GU/8111GU USB Ethernet Controller";

        default:
            return "Unknown/Undefined Realtek Device";
        }
    }

    const char *qualcomm_atheros_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x003E:
            return "Killer E2500 Gigabit Ethernet Controller";
        case 0x0042:
            return "QCA6174 802.11ac Wireless Network Adapter";
        case 0x0036:
            return "QCA9377 802.11ac Wireless Network Adapter";
        case 0x0041:
            return "Killer Wi-Fi 6 AX1650i 160MHz Wireless Network Adapter (201NGW)";
        case 0x0047:
            return "Killer Wi-Fi 6 AX1650x 160MHz Wireless Network Adapter (200D2W)";
        case 0x0046:
            return "Killer Wi-Fi 6 AX1650s 160MHz Wireless Network Adapter (200NGW)";
        case 0x0032:
            return "Killer E3000 2.5GbE Controller";
        case 0x003D:
            return "Killer E2400 Gigabit Ethernet Controller";
        case 0x0037:
            return "QCA9377 802.11ac Wireless Network Adapter";
        case 0x003F:
            return "Killer E2600 Gigabit Ethernet Controller";

        default:
            return "Unknown/Undefined Qualcomm Atheros Device";
        }
    }

    const char *texas_instruments_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x104C:
            return "XIO2000(A)/XIO2200A PCI Express-to-PCI Bridge";
        case 0x803B:
            return "TUSB8030/3 Universal Serial Bus Controller";
        case 0x8029:
            return "TNETE100P (PCL+)";
        case 0x8039:
            return "TNETE200P (PCL)";
        case 0x8139:
            return "TNETE2101P (PCL)";
        case 0x803A:
            return "TUSB6250 ATA Bridge";
        case 0x803C:
            return "TSB43AB21 IEEE-1394a-2000 Controller (PHY/Link)";
        case 0x8022:
            return "TMS320DM365 Digital Video";
        case 0x8231:
            return "XIO3130A PCI Express Switch";
        case 0x8021:
            return "TSB43AB22 IEEE-1394a-2000 Controller (PHY/Link )";

        default:
            return "Unknown/Undefined Texas Instruments Device";
        }
    }

    const char *via_technologies_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x3403:
            return "VL805 USB 3.0 Host Controller";
        case 0x3288:
            return "VT8251 AHCI/SATA 4-Port Controller";
        case 0x3204:
            return "VT6421 IDE RAID Controller";
        case 0x3124:
            return "VT6421 RAID Controller";
        case 0x0204:
            return "VT6306/7/8 [Fire II(M)] IEEE 1394 OHCI Controller";
        case 0x3287:
            return "VT8237/8251 Ultra/Standard AHCI Controller";
        case 0x3296:
            return "VT82xxxxx UHCI USB 1.1 Controller";
        case 0x3344:
            return "VT8237A/8251 SATA Controller";
        case 0x3349:
            return "VT6420 SATA RAID Controller";
        case 0x3189:
            return "VT8237 Bridge";

        default:
            return "Unknown/Undefined VIA Technologies Device";
        }
    }

    const char *marvell_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x4380:
            return "88W8363 [TopDog] 802.11n Wireless";
        case 0x43A0:
            return "88SE9125 PCIe SATA 6Gb/s Controller";
        case 0x4320:
            return "88E8036 PCI-E Fast Ethernet Controller";
        case 0x4355:
            return "88E8056 PCI-E Gigabit Ethernet Controller";
        case 0x4354:
            return "88E8053 PCI-E Gigabit Ethernet Controller";
        case 0x4321:
            return "88E8038 PCI-E Fast Ethernet Controller";
        case 0x4364:
            return "88E8072 PCI-E Gigabit Ethernet Controller";
        case 0x4340:
            return "88E8040 PCI-E Fast Ethernet Controller";
        case 0x436A:
            return "88E8057 PCI-E Gigabit Ethernet Controller";
        case 0x436B:
            return "88E8059 PCI-E Gigabit Ethernet Controller";

        default:
            return "Unknown/Undefined Marvell Technology Group Device";
        }
    }

    const char *asus_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x1043:
            return "RTL8111/8168/8411 Gigabit Ethernet Controller";
        case 0x0B05:
            return "Broadcom BCM20702 Bluetooth 4.0";
        case 0x1B21:
            return "ASM1061 SATA IDE Controller";
        case 0x293E:
            return "Xonar Essence STX";
        case 0x2A43:
            return "Xonar DGX";
        case 0x16EC:
            return "RTL8111/8168/8411 Gigabit Ethernet Controller";
        case 0x1C5C:
            return "USB2.0-CRW";
        case 0x15E3:
            return "Smart Gesture";
        case 0x1F03:
            return "ASUS Wi-Fi GO! card";
        case 0x3307:
            return "F5521gw Mobile Broadband Module";

        default:
            return "Unknown/Undefined ASUSTeK Computer Device";
        }
    }

    const char *dell_dev_name(uint16_t devID)
    {
        switch (devID)
        {
        case 0x0209:
            return "Integrated Webcam";
        case 0x1028:
            return "OptiPlex 755";
        case 0x3411:
            return "Dell U2711 Monitor Hub";
        case 0x3408:
            return "Integrated Touchpad / Trackstick";
        case 0x3405:
            return "Wireless 365 Bluetooth";
        case 0x3410:
            return "Integrated HD Audio";
        case 0x340E:
            return "Integrated Keyboard";
        case 0x3414:
            return "Integrated Smart Card Reader";
        case 0x3406:
            return "Wireless 365 Bluetooth Module";
        case 0x3418:
            return "Integrated Card Reader";

        default:
            return "Unknown/Undefined Dell Device";
        }
    }

    const char *GetDeviceName(uint16_t vendorID, uint16_t devID)
    {
        switch (vendorID)
        {
        case 0x8086:
            return intel_dev_name(devID);
        case 0x1022:
            return amd_dev_name(devID);
        case 0x10DE:
            return nvidia_dev_name(devID);
        case 0x10EC:
            return realtek_dev_name(devID);
        case 0x168C:
            return qualcomm_atheros_dev_name(devID);
        case 0x104C:
            return texas_instruments_dev_name(devID);
        case 0x1106:
            return via_technologies_dev_name(devID);
        case 0x1B4B:
            return marvell_dev_name(devID);
        case 0x1043:
            return asus_dev_name(devID);
        case 0x1028:
            return dell_dev_name(devID);

        default:
            return "Unknown/Undefined Device";
        }
    }

    // === SUBCLASS === //

    const char *UNC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "Non-VGA-Compatible Unclassified Device";
        case 0x01:
            return "VGA-Compatible Unclassified Device";
        }
        return "Unknown Device";
    }

    const char *MSC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "SCSI Bus Controller";
        case 0x01:
            return "IDE Controller";
        case 0x02:
            return "Floppy Disk Controller";
        case 0x03:
            return "IPI Bus Controller";
        case 0x04:
            return "RAID Controller";
        case 0x05:
            return "ATA Controller";
        case 0x06:
            return "Serial ATA Controller";
        case 0x07:
            return "Serial Attached SCSI Controller";
        case 0x08:
            return "Non-Volatile Memory Controller";
        case 0x80:
            return "Other Mass Storage Controller";
        }
        return "Unknown Device";
    }

    const char *NWC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "Ethernet Controller";
        case 0x01:
            return "Token Ring Controller";
        case 0x02:
            return "FDDI Controller";
        case 0x03:
            return "ATM Controller";
        case 0x04:
            return "ISDN Controller";
        case 0x05:
            return "WorldFip Controller";
        case 0x06:
            return "PICMG 2.14 Multi Computing Controller";
        case 0x07:
            return "Infiniband Controller";
        case 0x08:
            return "Fabric Controller";
        case 0x80:
            return "Other Networking Controller";
        }
        return "Unknown Device";
    }

    const char *DPC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "VGA Compatible Controller";
        case 0x01:
            return "XGA Controller";
        case 0x02:
            return "3D Controller";
        case 0x80:
            return "Other Display Controller";
        }
        return "Unknown Device";
    }

    const char *MuC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "Multimedia Video Controller";
        case 0x01:
            return "Multimedia Audio Controller";
        case 0x02:
            return "Computer Telephony Device";
        case 0x03:
            return "Audio Device";
        case 0x80:
            return "Other Multimedia Device";
        }
        return "Unknown Device";
    }

    const char *MMC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "RAM Controller";
        case 0x01:
            return "Flash Controller";
        case 0x80:
            return "Other Memory Controller";
        }
        return "Unknown Device";
    }

    const char *BRG_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "Host Bridge";
        case 0x01:
            return "ISA Bridge";
        case 0x02:
            return "EISA Bridge";
        case 0x03:
            return "MCA Bridge";
        case 0x04:
            return "PCI-to-PCI Bridge (0x04)";
        case 0x05:
            return "PCMCIA Bridge";
        case 0x06:
            return "NuBus Bridge";
        case 0x07:
            return "CardBus Bridge";
        case 0x08:
            return "RACEway Bridge";
        case 0x09:
            return "PCI-to-PCI Bridge (0x09)";
        case 0x0A:
            return "InfiniBand-to-PCI Host Bridge";
        case 0x80:
            return "Other Bridge Controller";
        }
        return "Unknown Device";
    }

    const char *SCC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "Serial Controller";
        case 0x01:
            return "Parallel Controller";
        case 0x02:
            return "Multiport Serial Controller";
        case 0x03:
            return "Modem";
        case 0x04:
            return "IEEE 488.1/2 (GPIB) Controller";
        case 0x05:
            return "Smart Card Controller";
        case 0x80:
            return "Other Simple COM Controller";
        }
        return "Unknown Device";
    }

    const char *BSP_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "PIC";
        case 0x01:
            return "DMA Controller";
        case 0x02:
            return "Timer";
        case 0x03:
            return "RTC Controller";
        case 0x04:
            return "PCI Hot-Plug Controller";
        case 0x05:
            return "SD Host controller";
        case 0x06:
            return "IOMMU";
        case 0x80:
            return "Other Base System Peripheral";
        }
        return "Unknown Device";
    }

    const char *IDC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "Keyboard Controller";
        case 0x01:
            return "Digitizer Pen";
        case 0x02:
            return "Mouse Controller";
        case 0x03:
            return "Scanner Controller";
        case 0x04:
            return "Gameport Controller";
        case 0x80:
            return "Other Input Device";
        }
        return "Unknown Device";
    }

    const char *PRC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "386";
        case 0x01:
            return "486";
        case 0x02:
            return "Pentium";
        case 0x03:
            return "Pentium Pro";
        case 0x10:
            return "Alpha";
        case 0x20:
            return "PowerPC";
        case 0x30:
            return "MIPS";
        case 0x40:
            return "Co-Processor";
        case 0x80:
            return "Other Processor / Co-Processor";
        }
        return "Unknown Device";
    }

    const char *SBC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "FireWire (IEEE 1394) Controller";
        case 0x01:
            return "ACCESS Bus Controller";
        case 0x02:
            return "SSA";
        case 0x03:
            return "USB Controller";
        case 0x04:
            return "Fibre Channel";
        case 0x05:
            return "SMBus Controller";
        case 0x06:
            return "InfiniBand Controller";
        case 0x07:
            return "IPMI Interface";
        case 0x08:
            return "SERCOS Interface (IEC 61491)";
        case 0x09:
            return "CANbus Controller";
        case 0x80:
            return "Other Serial Bus Controller";
        }
        return "Unknown Device";
    }

    const char *WRC_subclass_name(uint8_t subclass_code)
    {
        switch (subclass_code)
        {
        case 0x00:
            return "iRDA Compatible Controller";
        case 0x01:
            return "Consumer IR Controller";
        case 0x10:
            return "RF Controller";
        case 0x11:
            return "Bluetooth Controller";
        case 0x12:
            return "Broadband Controller";
        case 0x20:
            return "Ethernet Controller (802.1a)";
        case 0x21:
            return "Ethernet Controller (802.1b)";
        case 0x80:
            return "Other Wireless Controller";
        }
        return "Unknown Device";
    }

    const char *GetSubclassName(uint8_t class_code, uint8_t subclass_code)
    {
        switch (class_code)
        {
        case 0x00:
            return UNC_subclass_name(subclass_code); // Unclassified
        case 0x01:
            return MSC_subclass_name(subclass_code); // Mass Storage
        case 0x02:
            return NWC_subclass_name(subclass_code); // Networking
        case 0x03:
            return DPC_subclass_name(subclass_code); // Graphics/Display
        case 0x04:
            return MuC_subclass_name(subclass_code); // Multi Media
        case 0x05:
            return MMC_subclass_name(subclass_code); // Memory
        case 0x06:
            return BRG_subclass_name(subclass_code); // Bridges
        case 0x07:
            return SCC_subclass_name(subclass_code); // Simple COMs
        case 0x08:
            return SCC_subclass_name(subclass_code); // Base Sys Peripherals
        case 0x09:
            return IDC_subclass_name(subclass_code); // Input Devices
        case 0x0A:
            return "Docking Stations Not Supported"; // Docking Stations
        case 0x0B:
            return PRC_subclass_name(subclass_code); // Processors
        case 0x0C:
            return SBC_subclass_name(subclass_code); // Serial Bus Controllers
        case 0x0D:
            return WRC_subclass_name(subclass_code); // Wireless Controllers
        case 0x0E:
            return "I2O"; // Intelligent Controller
        }

        return "Unknown Subclass";
    }
}