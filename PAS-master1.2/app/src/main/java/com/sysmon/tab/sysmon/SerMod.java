package com.sysmon.tab.sysmon;

/**
 * Created by Tab on 2017-03-21.
 */

public class SerMod {

    public int DISK_us;
    public int DISK_fr;
    public long MEM_total;
    public long MEM_phy_F;
    public long MEM_cach_F;
    public long MEM_used;
    public double totalWload;
    public double wload1m;
    public double wload5m;
    public double wload15m;
    public double CPUload_us;
    public double CPUload_sy;
    public double CPUload_ni;
    public double CPUload_id;
    public double CPUload_wa;
    public int phy_cpu_num;
    public int log_cpu_num;
    public int cpu_num;

    public SerMod(String tmp)
    {
        String[] sers = tmp.split(";");

        DISK_us = Integer.parseInt(sers[0]);
        DISK_fr = Integer.parseInt(sers[1]);
        MEM_total = Long.parseLong(sers[2]);
        MEM_phy_F = Long.parseLong(sers[3]);
        MEM_cach_F = Long.parseLong(sers[4]);
        MEM_used = Long.parseLong(sers[5]);
        totalWload = Double.parseDouble(sers[6]);
        wload1m = Double.parseDouble(sers[7]);
        wload5m = Double.parseDouble(sers[8]);
        wload15m = Double.parseDouble(sers[9]);
        CPUload_us = Double.parseDouble(sers[10]);
        CPUload_sy = Double.parseDouble(sers[11]);
        CPUload_ni = Double.parseDouble(sers[12]);
        CPUload_id = Double.parseDouble(sers[13]);
        CPUload_wa = Double.parseDouble(sers[14]);
        phy_cpu_num = Integer.parseInt(sers[15]);
        log_cpu_num = Integer.parseInt(sers[16]);
        cpu_num = Integer.parseInt(sers[17]);
    }

    public SerMod()
    {
        DISK_us = 1;
        DISK_fr = 1;
        MEM_total = 1;
        MEM_phy_F = 1;
        MEM_cach_F = 1;
        MEM_used = 1;
        totalWload = 1;
        wload1m = 1;
        wload5m = 1;
        wload15m = 1;
        CPUload_us = 1;
        CPUload_sy = 1;
        CPUload_ni = 1;
        CPUload_id = 1;
        CPUload_wa = 1;
        phy_cpu_num = 1;
        log_cpu_num = 1;
        cpu_num = 1;
    }

    public SerMod(byte[] tmp)
    {

        DISK_us = (tmp[0] | tmp[1] << 8 | tmp[2] << 16 | tmp[3] << 24);
        DISK_fr = (tmp[4] | tmp[5] << 8 | tmp[6] << 16 | tmp[7] << 24);
        MEM_total = (tmp[8] | tmp[9] << 8 | tmp[10] << 16 | (long)tmp[11] << 24 | (long)tmp[12] << 32 | (long)tmp[13] << 40 | (long)tmp[14] << 48 | (long)tmp[15] << 56);
        MEM_phy_F = (tmp[16] | tmp[17] << 8 | tmp[18] << 16 | (long)tmp[19] << 24 | (long)tmp[20] << 32 | (long)tmp[21] << 40 | (long)tmp[22] << 48 | (long)tmp[23] << 56);
        MEM_cach_F = (tmp[24] | tmp[25] << 8 | tmp[26] << 16 | (long)tmp[27] << 24 | (long)tmp[28] << 32 | (long)tmp[29] << 40 | (long)tmp[30] << 48 | (long)tmp[31] << 56);
        MEM_used = (tmp[32] | tmp[33] << 8 | tmp[34] << 16 | (long)tmp[35] << 24 | (long)tmp[36] << 32 | (long)tmp[37] << 40 | (long)tmp[38] << 48 | (long)tmp[39] << 56);
        byte[] bt = new byte[8];
        System.arraycopy(tmp, 40, bt, 0, 8);
        totalWload = bytes2Double(bt);
        //totalWload = tmp[40] | tmp[41] << 8 | tmp[42] << 16 | (long)tmp[43] << 24 | (long)tmp[44] << 32 | (long)tmp[45] << 40 | (long)tmp[46] << 48 | (long)tmp[47] << 56);
        System.arraycopy(tmp, 48, bt, 0, 8);
        wload1m = bytes2Double(bt);
        //wload1m = Double.longBitsToDouble(tmp[48] | tmp[49] << 8 | tmp[50] << 16 | (long)tmp[51] << 24 | (long)tmp[52] << 32 | (long)tmp[53] << 40 | (long)tmp[54] << 48 | (long)tmp[55] << 56);
        System.arraycopy(tmp, 56, bt, 0, 8);
        wload5m = bytes2Double(bt);
        //wload5m = Double.longBitsToDouble(tmp[56] | tmp[57] << 8 | tmp[58] << 16 | (long)tmp[59] << 24 | (long)tmp[60] << 32 | (long)tmp[61] << 40 | (long)tmp[62] << 48 | (long)tmp[63] << 56);
        System.arraycopy(tmp, 64, bt, 0, 8);
        wload15m = bytes2Double(bt);
        //wload15m = Double.longBitsToDouble(tmp[64] | tmp[65] << 8 | tmp[66] << 16 | (long)tmp[67] << 24 | (long)tmp[68] << 32 | (long)tmp[69] << 40 | (long)tmp[70] << 48 | (long)tmp[71] << 56);
        System.arraycopy(tmp, 72, bt, 0, 8);
        CPUload_us = bytes2Double(bt);
        //CPUload_us = Double.longBitsToDouble(tmp[72] | tmp[73] << 8 | tmp[74] << 16 | (long)tmp[75] << 24 | (long)tmp[76] << 32 | (long)tmp[77] << 40 | (long)tmp[78] << 48 | (long)tmp[79] << 56);
        System.arraycopy(tmp, 80, bt, 0, 8);
        CPUload_sy = bytes2Double(bt);
        //CPUload_sy = Double.longBitsToDouble(tmp[80] | tmp[81] << 8 | tmp[82] << 16 | (long)tmp[83] << 24 | (long)tmp[84] << 32 | (long)tmp[85] << 40 | (long)tmp[86] << 48 | (long)tmp[87] << 56);
        System.arraycopy(tmp, 88, bt, 0, 8);
        CPUload_ni = bytes2Double(bt);
        //CPUload_ni = Double.longBitsToDouble(tmp[88] | tmp[89] << 8 | tmp[90] << 16 | (long)tmp[91] << 24 | (long)tmp[92] << 32 | (long)tmp[93] << 40 | (long)tmp[94] << 48 | (long)tmp[95] << 56);
        System.arraycopy(tmp, 96, bt, 0, 8);
        CPUload_id = bytes2Double(bt);
        //CPUload_id = Double.longBitsToDouble(tmp[96] | tmp[97] << 8 | tmp[98] << 16 | (long)tmp[99] << 24 | (long)tmp[100] << 32 | (long)tmp[101] << 40 | (long)tmp[102] << 48 | (long)tmp[103] << 56);
        System.arraycopy(tmp, 104, bt, 0, 8);
        CPUload_wa = bytes2Double(bt);
        //CPUload_wa = Double.longBitsToDouble(tmp[104] | tmp[105] << 8 | tmp[106] << 16 | (long)tmp[107] << 24 | (long)tmp[108] << 32 | (long)tmp[109] << 40 | (long)tmp[110] << 48 | (long)tmp[111] << 56);
        phy_cpu_num = (tmp[112] | tmp[113] << 8 | tmp[114] << 16 | tmp[115] << 24);
        log_cpu_num = (tmp[116] | tmp[117] << 8 | tmp[118] << 16 | tmp[119] << 24);
        cpu_num = (tmp[120] | tmp[121] << 8 | tmp[122] << 16 | tmp[123] << 24);
    }


    public static double bytes2Double(byte[] arr) {
        long value = 0;
        for (int i = 0; i < 8; i++) {
            value |= ((long) (arr[i] & 0xff)) << (8 * i);
        }
        return Double.longBitsToDouble(value);
    }

    public static byte[] double2Bytes(double d) {
        long value = Double.doubleToRawLongBits(d);
        byte[] byteRet = new byte[8];
        for (int i = 0; i < 8; i++) {
            byteRet[i] = (byte) ((value >> 8 * i) & 0xff);
        }
        return byteRet;
    }

    public String ToStringExt()
    {
        String result = "";

        result += (DISK_us + ";");
        result += (DISK_fr + ";");
        result += (MEM_total + ";");
        result += (MEM_phy_F + ";");
        result += (MEM_cach_F + ";");
        result += (MEM_used + ";");
        result += (totalWload + ";");
        result += (wload1m + ";");
        result += (wload5m + ";");
        result += (wload15m + ";");
        result += (CPUload_us + ";");
        result += (CPUload_sy + ";");
        result += (CPUload_ni + ";");
        result += (CPUload_id + ";");
        result += (CPUload_wa + ";");
        result += (phy_cpu_num + ";");
        result += (log_cpu_num + ";");
        result += (cpu_num + ";");

        return result;
    }
}
