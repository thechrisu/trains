#include "track_data.h"

void init_track(track_state *global_track) {
  track_node *track = global_track->track;
  turnout_state *turnouts = global_track->turnouts;
  tmemset(track, 0, (int)(TRACK_MAX*sizeof(track_node)));

#ifndef TESTING
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#endif /* TESTING */

  track[0].name = "A1";
  track[0].type = NODE_SENSOR;
  track[0].num = 0;
  track[0].reverse = &track[1];
  track[0].edge[DIR_AHEAD].reverse = &track[102].edge[DIR_STRAIGHT];
  track[0].edge[DIR_AHEAD].src = &track[0];
  track[0].edge[DIR_AHEAD].dest = &track[103];
  track[0].edge[DIR_AHEAD].dist = 231;
  track[1].name = "A2";
  track[1].type = NODE_SENSOR;
  track[1].num = 1;
  track[1].reverse = &track[0];
  track[1].edge[DIR_AHEAD].reverse = &track[132].edge[DIR_AHEAD];
  track[1].edge[DIR_AHEAD].src = &track[1];
  track[1].edge[DIR_AHEAD].dest = &track[133];
  track[1].edge[DIR_AHEAD].dist = 504;
  track[2].name = "A3";
  track[2].type = NODE_SENSOR;
  track[2].num = 2;
  track[2].reverse = &track[3];
  track[2].edge[DIR_AHEAD].reverse = &track[107].edge[DIR_AHEAD];
  track[2].edge[DIR_AHEAD].src = &track[2];
  track[2].edge[DIR_AHEAD].dest = &track[106];
  track[2].edge[DIR_AHEAD].dist = 43;
  track[3].name = "A4";
  track[3].type = NODE_SENSOR;
  track[3].num = 3;
  track[3].reverse = &track[2];
  track[3].edge[DIR_AHEAD].reverse = &track[30].edge[DIR_AHEAD];
  track[3].edge[DIR_AHEAD].src = &track[3];
  track[3].edge[DIR_AHEAD].dest = &track[31];
  track[3].edge[DIR_AHEAD].dist = 437;
  track[4].name = "A5";
  track[4].type = NODE_SENSOR;
  track[4].num = 4;
  track[4].reverse = &track[5];
  track[4].edge[DIR_AHEAD].reverse = &track[84].edge[DIR_STRAIGHT];
  track[4].edge[DIR_AHEAD].src = &track[4];
  track[4].edge[DIR_AHEAD].dest = &track[85];
  track[4].edge[DIR_AHEAD].dist = 231;
  track[5].name = "A6";
  track[5].type = NODE_SENSOR;
  track[5].num = 5;
  track[5].reverse = &track[4];
  track[5].edge[DIR_AHEAD].reverse = &track[24].edge[DIR_AHEAD];
  track[5].edge[DIR_AHEAD].src = &track[5];
  track[5].edge[DIR_AHEAD].dest = &track[25];
  track[5].edge[DIR_AHEAD].dist = 642;
  track[6].name = "A7";
  track[6].type = NODE_SENSOR;
  track[6].num = 6;
  track[6].reverse = &track[7];
  track[6].edge[DIR_AHEAD].reverse = &track[26].edge[DIR_AHEAD];
  track[6].edge[DIR_AHEAD].src = &track[6];
  track[6].edge[DIR_AHEAD].dest = &track[27];
  track[6].edge[DIR_AHEAD].dist = 470;
  track[7].name = "A8";
  track[7].type = NODE_SENSOR;
  track[7].num = 7;
  track[7].reverse = &track[6];
  track[7].edge[DIR_AHEAD].reverse = &track[82].edge[DIR_CURVED];
  track[7].edge[DIR_AHEAD].src = &track[7];
  track[7].edge[DIR_AHEAD].dest = &track[83];
  track[7].edge[DIR_AHEAD].dist = 229;
  track[8].name = "A9";
  track[8].type = NODE_SENSOR;
  track[8].num = 8;
  track[8].reverse = &track[9];
  track[8].edge[DIR_AHEAD].reverse = &track[22].edge[DIR_AHEAD];
  track[8].edge[DIR_AHEAD].src = &track[8];
  track[8].edge[DIR_AHEAD].dest = &track[23];
  track[8].edge[DIR_AHEAD].dist = 289;
  track[9].name = "A10";
  track[9].type = NODE_SENSOR;
  track[9].num = 9;
  track[9].reverse = &track[8];
  track[9].edge[DIR_AHEAD].reverse = &track[80].edge[DIR_CURVED];
  track[9].edge[DIR_AHEAD].src = &track[9];
  track[9].edge[DIR_AHEAD].dest = &track[81];
  track[9].edge[DIR_AHEAD].dist = 229;
  track[10].name = "A11";
  track[10].type = NODE_SENSOR;
  track[10].num = 10;
  track[10].reverse = &track[11];
  track[10].edge[DIR_AHEAD].reverse = &track[80].edge[DIR_STRAIGHT];
  track[10].edge[DIR_AHEAD].src = &track[10];
  track[10].edge[DIR_AHEAD].dest = &track[81];
  track[10].edge[DIR_AHEAD].dist = 518;
  track[11].name = "A12";
  track[11].type = NODE_SENSOR;
  track[11].num = 11;
  track[11].reverse = &track[10];
  track[11].edge[DIR_AHEAD].reverse = &track[138].edge[DIR_AHEAD];
  track[11].edge[DIR_AHEAD].src = &track[11];
  track[11].edge[DIR_AHEAD].dest = &track[139];
  track[11].edge[DIR_AHEAD].dist = 43;
  track[12].name = "A13";
  track[12].type = NODE_SENSOR;
  track[12].num = 12;
  track[12].reverse = &track[13];
  track[12].edge[DIR_AHEAD].reverse = &track[86].edge[DIR_CURVED];
  track[12].edge[DIR_AHEAD].src = &track[12];
  track[12].edge[DIR_AHEAD].dest = &track[87];
  track[12].edge[DIR_AHEAD].dist = 236;
  track[13].name = "A14";
  track[13].type = NODE_SENSOR;
  track[13].num = 13;
  track[13].reverse = &track[12];
  track[13].edge[DIR_AHEAD].reverse = &track[130].edge[DIR_AHEAD];
  track[13].edge[DIR_AHEAD].src = &track[13];
  track[13].edge[DIR_AHEAD].dest = &track[131];
  track[13].edge[DIR_AHEAD].dist = 325;
  track[14].name = "A15";
  track[14].type = NODE_SENSOR;
  track[14].num = 14;
  track[14].reverse = &track[15];
  track[14].edge[DIR_AHEAD].reverse = &track[134].edge[DIR_AHEAD];
  track[14].edge[DIR_AHEAD].src = &track[14];
  track[14].edge[DIR_AHEAD].dest = &track[135];
  track[14].edge[DIR_AHEAD].dist = 144;
  track[15].name = "A16";
  track[15].type = NODE_SENSOR;
  track[15].num = 15;
  track[15].reverse = &track[14];
  track[15].edge[DIR_AHEAD].reverse = &track[86].edge[DIR_STRAIGHT];
  track[15].edge[DIR_AHEAD].src = &track[15];
  track[15].edge[DIR_AHEAD].dest = &track[87];
  track[15].edge[DIR_AHEAD].dist = 417;
  track[16].name = "B1";
  track[16].type = NODE_SENSOR;
  track[16].num = 16;
  track[16].reverse = &track[17];
  track[16].edge[DIR_AHEAD].reverse = &track[60].edge[DIR_AHEAD];
  track[16].edge[DIR_AHEAD].src = &track[16];
  track[16].edge[DIR_AHEAD].dest = &track[61];
  track[16].edge[DIR_AHEAD].dist = 404;
  track[17].name = "B2";
  track[17].type = NODE_SENSOR;
  track[17].num = 17;
  track[17].reverse = &track[16];
  track[17].edge[DIR_AHEAD].reverse = &track[110].edge[DIR_STRAIGHT];
  track[17].edge[DIR_AHEAD].src = &track[17];
  track[17].edge[DIR_AHEAD].dest = &track[111];
  track[17].edge[DIR_AHEAD].dist = 231;
  track[18].name = "B3";
  track[18].type = NODE_SENSOR;
  track[18].num = 18;
  track[18].reverse = &track[19];
  track[18].edge[DIR_AHEAD].reverse = &track[32].edge[DIR_AHEAD];
  track[18].edge[DIR_AHEAD].src = &track[18];
  track[18].edge[DIR_AHEAD].dest = &track[33];
  track[18].edge[DIR_AHEAD].dist = 201;
  track[19].name = "B4";
  track[19].type = NODE_SENSOR;
  track[19].num = 19;
  track[19].reverse = &track[18];
  track[19].edge[DIR_AHEAD].reverse = &track[110].edge[DIR_CURVED];
  track[19].edge[DIR_AHEAD].src = &track[19];
  track[19].edge[DIR_AHEAD].dest = &track[111];
  track[19].edge[DIR_AHEAD].dist = 239;
  track[20].name = "B5";
  track[20].type = NODE_SENSOR;
  track[20].num = 20;
  track[20].reverse = &track[21];
  track[20].edge[DIR_AHEAD].reverse = &track[51].edge[DIR_AHEAD];
  track[20].edge[DIR_AHEAD].src = &track[20];
  track[20].edge[DIR_AHEAD].dest = &track[50];
  track[20].edge[DIR_AHEAD].dist = 404;
  track[21].name = "B6";
  track[21].type = NODE_SENSOR;
  track[21].num = 21;
  track[21].reverse = &track[20];
  track[21].edge[DIR_AHEAD].reverse = &track[104].edge[DIR_STRAIGHT];
  track[21].edge[DIR_AHEAD].src = &track[21];
  track[21].edge[DIR_AHEAD].dest = &track[105];
  track[21].edge[DIR_AHEAD].dist = 231;
  track[22].name = "B7";
  track[22].type = NODE_SENSOR;
  track[22].num = 22;
  track[22].reverse = &track[23];
  track[22].edge[DIR_AHEAD].reverse = &track[8].edge[DIR_AHEAD];
  track[22].edge[DIR_AHEAD].src = &track[22];
  track[22].edge[DIR_AHEAD].dest = &track[9];
  track[22].edge[DIR_AHEAD].dist = 289;
  track[23].name = "B8";
  track[23].type = NODE_SENSOR;
  track[23].num = 23;
  track[23].reverse = &track[22];
  track[23].edge[DIR_AHEAD].reverse = &track[136].edge[DIR_AHEAD];
  track[23].edge[DIR_AHEAD].src = &track[23];
  track[23].edge[DIR_AHEAD].dest = &track[137];
  track[23].edge[DIR_AHEAD].dist = 43;
  track[24].name = "B9";
  track[24].type = NODE_SENSOR;
  track[24].num = 24;
  track[24].reverse = &track[25];
  track[24].edge[DIR_AHEAD].reverse = &track[5].edge[DIR_AHEAD];
  track[24].edge[DIR_AHEAD].src = &track[24];
  track[24].edge[DIR_AHEAD].dest = &track[4];
  track[24].edge[DIR_AHEAD].dist = 642;
  track[25].name = "B10";
  track[25].type = NODE_SENSOR;
  track[25].num = 25;
  track[25].reverse = &track[24];
  track[25].edge[DIR_AHEAD].reverse = &track[140].edge[DIR_AHEAD];
  track[25].edge[DIR_AHEAD].src = &track[25];
  track[25].edge[DIR_AHEAD].dest = &track[141];
  track[25].edge[DIR_AHEAD].dist = 50;
  track[26].name = "B11";
  track[26].type = NODE_SENSOR;
  track[26].num = 26;
  track[26].reverse = &track[27];
  track[26].edge[DIR_AHEAD].reverse = &track[6].edge[DIR_AHEAD];
  track[26].edge[DIR_AHEAD].src = &track[26];
  track[26].edge[DIR_AHEAD].dest = &track[7];
  track[26].edge[DIR_AHEAD].dist = 470;
  track[27].name = "B12";
  track[27].type = NODE_SENSOR;
  track[27].num = 27;
  track[27].reverse = &track[26];
  track[27].edge[DIR_AHEAD].reverse = &track[142].edge[DIR_AHEAD];
  track[27].edge[DIR_AHEAD].src = &track[27];
  track[27].edge[DIR_AHEAD].dest = &track[143];
  track[27].edge[DIR_AHEAD].dist = 50;
  track[28].name = "B13";
  track[28].type = NODE_SENSOR;
  track[28].num = 28;
  track[28].reverse = &track[29];
  track[28].edge[DIR_AHEAD].reverse = &track[118].edge[DIR_CURVED];
  track[28].edge[DIR_AHEAD].src = &track[28];
  track[28].edge[DIR_AHEAD].dest = &track[119];
  track[28].edge[DIR_AHEAD].dist = 239;
  track[29].name = "B14";
  track[29].type = NODE_SENSOR;
  track[29].num = 29;
  track[29].reverse = &track[28];
  track[29].edge[DIR_AHEAD].reverse = &track[62].edge[DIR_AHEAD];
  track[29].edge[DIR_AHEAD].src = &track[29];
  track[29].edge[DIR_AHEAD].dest = &track[63];
  track[29].edge[DIR_AHEAD].dist = 201;
  track[30].name = "B15";
  track[30].type = NODE_SENSOR;
  track[30].num = 30;
  track[30].reverse = &track[31];
  track[30].edge[DIR_AHEAD].reverse = &track[3].edge[DIR_AHEAD];
  track[30].edge[DIR_AHEAD].src = &track[30];
  track[30].edge[DIR_AHEAD].dest = &track[2];
  track[30].edge[DIR_AHEAD].dist = 437;
  track[31].name = "B16";
  track[31].type = NODE_SENSOR;
  track[31].num = 31;
  track[31].reverse = &track[30];
  track[31].edge[DIR_AHEAD].reverse = &track[109].edge[DIR_AHEAD];
  track[31].edge[DIR_AHEAD].src = &track[31];
  track[31].edge[DIR_AHEAD].dest = &track[108];
  track[31].edge[DIR_AHEAD].dist = 50;
  track[32].name = "C1";
  track[32].type = NODE_SENSOR;
  track[32].num = 32;
  track[32].reverse = &track[33];
  track[32].edge[DIR_AHEAD].reverse = &track[18].edge[DIR_AHEAD];
  track[32].edge[DIR_AHEAD].src = &track[32];
  track[32].edge[DIR_AHEAD].dest = &track[19];
  track[32].edge[DIR_AHEAD].dist = 201;
  track[33].name = "C2";
  track[33].type = NODE_SENSOR;
  track[33].num = 33;
  track[33].reverse = &track[32];
  track[33].edge[DIR_AHEAD].reverse = &track[116].edge[DIR_CURVED];
  track[33].edge[DIR_AHEAD].src = &track[33];
  track[33].edge[DIR_AHEAD].dest = &track[117];
  track[33].edge[DIR_AHEAD].dist = 246;
  track[34].name = "C3";
  track[34].type = NODE_SENSOR;
  track[34].num = 34;
  track[34].reverse = &track[35];
  track[34].edge[DIR_AHEAD].reverse = &track[128].edge[DIR_AHEAD];
  track[34].edge[DIR_AHEAD].src = &track[34];
  track[34].edge[DIR_AHEAD].dest = &track[129];
  track[34].edge[DIR_AHEAD].dist = 514;
  track[35].name = "C4";
  track[35].type = NODE_SENSOR;
  track[35].num = 35;
  track[35].reverse = &track[34];
  track[35].edge[DIR_AHEAD].reverse = &track[88].edge[DIR_STRAIGHT];
  track[35].edge[DIR_AHEAD].src = &track[35];
  track[35].edge[DIR_AHEAD].dest = &track[89];
  track[35].edge[DIR_AHEAD].dist = 239;
  track[36].name = "C5";
  track[36].type = NODE_SENSOR;
  track[36].num = 36;
  track[36].reverse = &track[37];
  track[36].edge[DIR_AHEAD].reverse = &track[91].edge[DIR_AHEAD];
  track[36].edge[DIR_AHEAD].src = &track[36];
  track[36].edge[DIR_AHEAD].dest = &track[90];
  track[36].edge[DIR_AHEAD].dist = 61;
  track[37].name = "C6";
  track[37].type = NODE_SENSOR;
  track[37].num = 37;
  track[37].reverse = &track[36];
  track[37].edge[DIR_AHEAD].reverse = &track[108].edge[DIR_STRAIGHT];
  track[37].edge[DIR_AHEAD].src = &track[37];
  track[37].edge[DIR_AHEAD].dest = &track[109];
  track[37].edge[DIR_AHEAD].dist = 433;
  track[38].name = "C7";
  track[38].type = NODE_SENSOR;
  track[38].num = 38;
  track[38].reverse = &track[39];
  track[38].edge[DIR_AHEAD].reverse = &track[114].edge[DIR_STRAIGHT];
  track[38].edge[DIR_AHEAD].src = &track[38];
  track[38].edge[DIR_AHEAD].dest = &track[115];
  track[38].edge[DIR_AHEAD].dist = 231;
  track[39].name = "C8";
  track[39].type = NODE_SENSOR;
  track[39].num = 39;
  track[39].reverse = &track[38];
  track[39].edge[DIR_AHEAD].reverse = &track[85].edge[DIR_AHEAD];
  track[39].edge[DIR_AHEAD].src = &track[39];
  track[39].edge[DIR_AHEAD].dest = &track[84];
  track[39].edge[DIR_AHEAD].dist = 128;
  track[40].name = "C9";
  track[40].type = NODE_SENSOR;
  track[40].num = 40;
  track[40].reverse = &track[41];
  track[40].edge[DIR_AHEAD].reverse = &track[108].edge[DIR_CURVED];
  track[40].edge[DIR_AHEAD].src = &track[40];
  track[40].edge[DIR_AHEAD].dest = &track[109];
  track[40].edge[DIR_AHEAD].dist = 326;
  track[41].name = "C10";
  track[41].type = NODE_SENSOR;
  track[41].num = 41;
  track[41].reverse = &track[40];
  track[41].edge[DIR_AHEAD].reverse = &track[111].edge[DIR_AHEAD];
  track[41].edge[DIR_AHEAD].src = &track[41];
  track[41].edge[DIR_AHEAD].dest = &track[110];
  track[41].edge[DIR_AHEAD].dist = 128;
  track[42].name = "C11";
  track[42].type = NODE_SENSOR;
  track[42].num = 42;
  track[42].reverse = &track[43];
  track[42].edge[DIR_AHEAD].reverse = &track[105].edge[DIR_AHEAD];
  track[42].edge[DIR_AHEAD].src = &track[42];
  track[42].edge[DIR_AHEAD].dest = &track[104];
  track[42].edge[DIR_AHEAD].dist = 120;
  track[43].name = "C12";
  track[43].type = NODE_SENSOR;
  track[43].num = 43;
  track[43].reverse = &track[42];
  track[43].edge[DIR_AHEAD].reverse = &track[106].edge[DIR_CURVED];
  track[43].edge[DIR_AHEAD].src = &track[43];
  track[43].edge[DIR_AHEAD].dest = &track[107];
  track[43].edge[DIR_AHEAD].dist = 333;
  track[44].name = "C13";
  track[44].type = NODE_SENSOR;
  track[44].num = 44;
  track[44].reverse = &track[45];
  track[44].edge[DIR_AHEAD].reverse = &track[71].edge[DIR_AHEAD];
  track[44].edge[DIR_AHEAD].src = &track[44];
  track[44].edge[DIR_AHEAD].dest = &track[70];
  track[44].edge[DIR_AHEAD].dist = 875;
  track[45].name = "C14";
  track[45].type = NODE_SENSOR;
  track[45].num = 45;
  track[45].reverse = &track[44];
  track[45].edge[DIR_AHEAD].reverse = &track[101].edge[DIR_AHEAD];
  track[45].edge[DIR_AHEAD].src = &track[45];
  track[45].edge[DIR_AHEAD].dest = &track[100];
  track[45].edge[DIR_AHEAD].dist = 43;
  track[46].name = "C15";
  track[46].type = NODE_SENSOR;
  track[46].num = 46;
  track[46].reverse = &track[47];
  track[46].edge[DIR_AHEAD].reverse = &track[58].edge[DIR_AHEAD];
  track[46].edge[DIR_AHEAD].src = &track[46];
  track[46].edge[DIR_AHEAD].dest = &track[59];
  track[46].edge[DIR_AHEAD].dist = 404;
  track[47].name = "C16";
  track[47].type = NODE_SENSOR;
  track[47].num = 47;
  track[47].reverse = &track[46];
  track[47].edge[DIR_AHEAD].reverse = &track[90].edge[DIR_STRAIGHT];
  track[47].edge[DIR_AHEAD].src = &track[47];
  track[47].edge[DIR_AHEAD].dest = &track[91];
  track[47].edge[DIR_AHEAD].dist = 239;
  track[48].name = "D1";
  track[48].type = NODE_SENSOR;
  track[48].num = 48;
  track[48].reverse = &track[49];
  track[48].edge[DIR_AHEAD].reverse = &track[120].edge[DIR_CURVED];
  track[48].edge[DIR_AHEAD].src = &track[48];
  track[48].edge[DIR_AHEAD].dest = &track[121];
  track[48].edge[DIR_AHEAD].dist = 246;
  track[49].name = "D2";
  track[49].type = NODE_SENSOR;
  track[49].num = 49;
  track[49].reverse = &track[48];
  track[49].edge[DIR_AHEAD].reverse = &track[66].edge[DIR_AHEAD];
  track[49].edge[DIR_AHEAD].src = &track[49];
  track[49].edge[DIR_AHEAD].dest = &track[67];
  track[49].edge[DIR_AHEAD].dist = 201;
  track[50].name = "D3";
  track[50].type = NODE_SENSOR;
  track[50].num = 50;
  track[50].reverse = &track[51];
  track[50].edge[DIR_AHEAD].reverse = &track[98].edge[DIR_STRAIGHT];
  track[50].edge[DIR_AHEAD].src = &track[50];
  track[50].edge[DIR_AHEAD].dest = &track[99];
  track[50].edge[DIR_AHEAD].dist = 239;
  track[51].name = "D4";
  track[51].type = NODE_SENSOR;
  track[51].num = 51;
  track[51].reverse = &track[50];
  track[51].edge[DIR_AHEAD].reverse = &track[20].edge[DIR_AHEAD];
  track[51].edge[DIR_AHEAD].src = &track[51];
  track[51].edge[DIR_AHEAD].dest = &track[21];
  track[51].edge[DIR_AHEAD].dist = 404;
  track[52].name = "D5";
  track[52].type = NODE_SENSOR;
  track[52].num = 52;
  track[52].reverse = &track[53];
  track[52].edge[DIR_AHEAD].reverse = &track[68].edge[DIR_AHEAD];
  track[52].edge[DIR_AHEAD].src = &track[52];
  track[52].edge[DIR_AHEAD].dest = &track[69];
  track[52].edge[DIR_AHEAD].dist = 376;
  track[53].name = "D6";
  track[53].type = NODE_SENSOR;
  track[53].num = 53;
  track[53].reverse = &track[52];
  track[53].edge[DIR_AHEAD].reverse = &track[96].edge[DIR_CURVED];
  track[53].edge[DIR_AHEAD].src = &track[53];
  track[53].edge[DIR_AHEAD].dest = &track[97];
  track[53].edge[DIR_AHEAD].dist = 239;
  track[54].name = "D7";
  track[54].type = NODE_SENSOR;
  track[54].num = 54;
  track[54].reverse = &track[55];
  track[54].edge[DIR_AHEAD].reverse = &track[96].edge[DIR_STRAIGHT];
  track[54].edge[DIR_AHEAD].src = &track[54];
  track[54].edge[DIR_AHEAD].dest = &track[97];
  track[54].edge[DIR_AHEAD].dist = 309;
  track[55].name = "D8";
  track[55].type = NODE_SENSOR;
  track[55].num = 55;
  track[55].reverse = &track[54];
  track[55].edge[DIR_AHEAD].reverse = &track[70].edge[DIR_AHEAD];
  track[55].edge[DIR_AHEAD].src = &track[55];
  track[55].edge[DIR_AHEAD].dest = &track[71];
  track[55].edge[DIR_AHEAD].dist = 384;
  track[56].name = "D9";
  track[56].type = NODE_SENSOR;
  track[56].num = 56;
  track[56].reverse = &track[57];
  track[56].edge[DIR_AHEAD].reverse = &track[74].edge[DIR_AHEAD];
  track[56].edge[DIR_AHEAD].src = &track[56];
  track[56].edge[DIR_AHEAD].dest = &track[75];
  track[56].edge[DIR_AHEAD].dist = 369;
  track[57].name = "D10";
  track[57].type = NODE_SENSOR;
  track[57].num = 57;
  track[57].reverse = &track[56];
  track[57].edge[DIR_AHEAD].reverse = &track[94].edge[DIR_STRAIGHT];
  track[57].edge[DIR_AHEAD].src = &track[57];
  track[57].edge[DIR_AHEAD].dest = &track[95];
  track[57].edge[DIR_AHEAD].dist = 316;
  track[58].name = "D11";
  track[58].type = NODE_SENSOR;
  track[58].num = 58;
  track[58].reverse = &track[59];
  track[58].edge[DIR_AHEAD].reverse = &track[46].edge[DIR_AHEAD];
  track[58].edge[DIR_AHEAD].src = &track[58];
  track[58].edge[DIR_AHEAD].dest = &track[47];
  track[58].edge[DIR_AHEAD].dist = 404;
  track[59].name = "D12";
  track[59].type = NODE_SENSOR;
  track[59].num = 59;
  track[59].reverse = &track[58];
  track[59].edge[DIR_AHEAD].reverse = &track[92].edge[DIR_STRAIGHT];
  track[59].edge[DIR_AHEAD].src = &track[59];
  track[59].edge[DIR_AHEAD].dest = &track[93];
  track[59].edge[DIR_AHEAD].dist = 231;
  track[60].name = "D13";
  track[60].type = NODE_SENSOR;
  track[60].num = 60;
  track[60].reverse = &track[61];
  track[60].edge[DIR_AHEAD].reverse = &track[16].edge[DIR_AHEAD];
  track[60].edge[DIR_AHEAD].src = &track[60];
  track[60].edge[DIR_AHEAD].dest = &track[17];
  track[60].edge[DIR_AHEAD].dist = 404;
  track[61].name = "D14";
  track[61].type = NODE_SENSOR;
  track[61].num = 61;
  track[61].reverse = &track[60];
  track[61].edge[DIR_AHEAD].reverse = &track[112].edge[DIR_STRAIGHT];
  track[61].edge[DIR_AHEAD].src = &track[61];
  track[61].edge[DIR_AHEAD].dest = &track[113];
  track[61].edge[DIR_AHEAD].dist = 239;
  track[62].name = "D15";
  track[62].type = NODE_SENSOR;
  track[62].num = 62;
  track[62].reverse = &track[63];
  track[62].edge[DIR_AHEAD].reverse = &track[29].edge[DIR_AHEAD];
  track[62].edge[DIR_AHEAD].src = &track[62];
  track[62].edge[DIR_AHEAD].dest = &track[28];
  track[62].edge[DIR_AHEAD].dist = 201;
  track[63].name = "D16";
  track[63].type = NODE_SENSOR;
  track[63].num = 63;
  track[63].reverse = &track[62];
  track[63].edge[DIR_AHEAD].reverse = &track[112].edge[DIR_CURVED];
  track[63].edge[DIR_AHEAD].src = &track[63];
  track[63].edge[DIR_AHEAD].dest = &track[113];
  track[63].edge[DIR_AHEAD].dist = 246;
  track[64].name = "E1";
  track[64].type = NODE_SENSOR;
  track[64].num = 64;
  track[64].reverse = &track[65];
  track[64].edge[DIR_AHEAD].reverse = &track[122].edge[DIR_CURVED];
  track[64].edge[DIR_AHEAD].src = &track[64];
  track[64].edge[DIR_AHEAD].dest = &track[123];
  track[64].edge[DIR_AHEAD].dist = 239;
  track[65].name = "E2";
  track[65].type = NODE_SENSOR;
  track[65].num = 65;
  track[65].reverse = &track[64];
  track[65].edge[DIR_AHEAD].reverse = &track[79].edge[DIR_AHEAD];
  track[65].edge[DIR_AHEAD].src = &track[65];
  track[65].edge[DIR_AHEAD].dest = &track[78];
  track[65].edge[DIR_AHEAD].dist = 201;
  track[66].name = "E3";
  track[66].type = NODE_SENSOR;
  track[66].num = 66;
  track[66].reverse = &track[67];
  track[66].edge[DIR_AHEAD].reverse = &track[49].edge[DIR_AHEAD];
  track[66].edge[DIR_AHEAD].src = &track[66];
  track[66].edge[DIR_AHEAD].dest = &track[48];
  track[66].edge[DIR_AHEAD].dist = 201;
  track[67].name = "E4";
  track[67].type = NODE_SENSOR;
  track[67].num = 67;
  track[67].reverse = &track[66];
  track[67].edge[DIR_AHEAD].reverse = &track[98].edge[DIR_CURVED];
  track[67].edge[DIR_AHEAD].src = &track[67];
  track[67].edge[DIR_AHEAD].dest = &track[99];
  track[67].edge[DIR_AHEAD].dist = 239;
  track[68].name = "E5";
  track[68].type = NODE_SENSOR;
  track[68].num = 68;
  track[68].reverse = &track[69];
  track[68].edge[DIR_AHEAD].reverse = &track[52].edge[DIR_AHEAD];
  track[68].edge[DIR_AHEAD].src = &track[68];
  track[68].edge[DIR_AHEAD].dest = &track[53];
  track[68].edge[DIR_AHEAD].dist = 376;
  track[69].name = "E6";
  track[69].type = NODE_SENSOR;
  track[69].num = 69;
  track[69].reverse = &track[68];
  track[69].edge[DIR_AHEAD].reverse = &track[99].edge[DIR_AHEAD];
  track[69].edge[DIR_AHEAD].src = &track[69];
  track[69].edge[DIR_AHEAD].dest = &track[98];
  track[69].edge[DIR_AHEAD].dist = 50;
  track[70].name = "E7";
  track[70].type = NODE_SENSOR;
  track[70].num = 70;
  track[70].reverse = &track[71];
  track[70].edge[DIR_AHEAD].reverse = &track[55].edge[DIR_AHEAD];
  track[70].edge[DIR_AHEAD].src = &track[70];
  track[70].edge[DIR_AHEAD].dest = &track[54];
  track[70].edge[DIR_AHEAD].dist = 384;
  track[71].name = "E8";
  track[71].type = NODE_SENSOR;
  track[71].num = 71;
  track[71].reverse = &track[70];
  track[71].edge[DIR_AHEAD].reverse = &track[44].edge[DIR_AHEAD];
  track[71].edge[DIR_AHEAD].src = &track[71];
  track[71].edge[DIR_AHEAD].dest = &track[45];
  track[71].edge[DIR_AHEAD].dist = 875;
  track[72].name = "E9";
  track[72].type = NODE_SENSOR;
  track[72].num = 72;
  track[72].reverse = &track[73];
  track[72].edge[DIR_AHEAD].reverse = &track[94].edge[DIR_CURVED];
  track[72].edge[DIR_AHEAD].src = &track[72];
  track[72].edge[DIR_AHEAD].dest = &track[95];
  track[72].edge[DIR_AHEAD].dist = 239;
  track[73].name = "E10";
  track[73].type = NODE_SENSOR;
  track[73].num = 73;
  track[73].reverse = &track[72];
  track[73].edge[DIR_AHEAD].reverse = &track[77].edge[DIR_AHEAD];
  track[73].edge[DIR_AHEAD].src = &track[73];
  track[73].edge[DIR_AHEAD].dest = &track[76];
  track[73].edge[DIR_AHEAD].dist = 376;
  track[74].name = "E11";
  track[74].type = NODE_SENSOR;
  track[74].num = 74;
  track[74].reverse = &track[75];
  track[74].edge[DIR_AHEAD].reverse = &track[56].edge[DIR_AHEAD];
  track[74].edge[DIR_AHEAD].src = &track[74];
  track[74].edge[DIR_AHEAD].dest = &track[57];
  track[74].edge[DIR_AHEAD].dist = 369;
  track[75].name = "E12";
  track[75].type = NODE_SENSOR;
  track[75].num = 75;
  track[75].reverse = &track[74];
  track[75].edge[DIR_AHEAD].reverse = &track[93].edge[DIR_AHEAD];
  track[75].edge[DIR_AHEAD].src = &track[75];
  track[75].edge[DIR_AHEAD].dest = &track[92];
  track[75].edge[DIR_AHEAD].dist = 50;
  track[76].name = "E13";
  track[76].type = NODE_SENSOR;
  track[76].num = 76;
  track[76].reverse = &track[77];
  track[76].edge[DIR_AHEAD].reverse = &track[113].edge[DIR_AHEAD];
  track[76].edge[DIR_AHEAD].src = &track[76];
  track[76].edge[DIR_AHEAD].dest = &track[112];
  track[76].edge[DIR_AHEAD].dist = 43;
  track[77].name = "E14";
  track[77].type = NODE_SENSOR;
  track[77].num = 77;
  track[77].reverse = &track[76];
  track[77].edge[DIR_AHEAD].reverse = &track[73].edge[DIR_AHEAD];
  track[77].edge[DIR_AHEAD].src = &track[77];
  track[77].edge[DIR_AHEAD].dest = &track[72];
  track[77].edge[DIR_AHEAD].dist = 376;
  track[78].name = "E15";
  track[78].type = NODE_SENSOR;
  track[78].num = 78;
  track[78].reverse = &track[79];
  track[78].edge[DIR_AHEAD].reverse = &track[104].edge[DIR_CURVED];
  track[78].edge[DIR_AHEAD].src = &track[78];
  track[78].edge[DIR_AHEAD].dest = &track[105];
  track[78].edge[DIR_AHEAD].dist = 246;
  track[79].name = "E16";
  track[79].type = NODE_SENSOR;
  track[79].num = 79;
  track[79].reverse = &track[78];
  track[79].edge[DIR_AHEAD].reverse = &track[65].edge[DIR_AHEAD];
  track[79].edge[DIR_AHEAD].src = &track[79];
  track[79].edge[DIR_AHEAD].dest = &track[64];
  track[79].edge[DIR_AHEAD].dist = 201;
  track[80].name = "BR1";
  track[80].type = NODE_BRANCH;
  track[80].num = 1;
  track[80].reverse = &track[81];
  track[80].edge[DIR_STRAIGHT].reverse = &track[10].edge[DIR_AHEAD];
  track[80].edge[DIR_STRAIGHT].src = &track[80];
  track[80].edge[DIR_STRAIGHT].dest = &track[11];
  track[80].edge[DIR_STRAIGHT].dist = 518;
  track[80].edge[DIR_CURVED].reverse = &track[9].edge[DIR_AHEAD];
  track[80].edge[DIR_CURVED].src = &track[80];
  track[80].edge[DIR_CURVED].dest = &track[8];
  track[80].edge[DIR_CURVED].dist = 229;
  track[81].name = "MR1";
  track[81].type = NODE_MERGE;
  track[81].num = 1;
  track[81].reverse = &track[80];
  track[81].edge[DIR_AHEAD].reverse = &track[82].edge[DIR_STRAIGHT];
  track[81].edge[DIR_AHEAD].src = &track[81];
  track[81].edge[DIR_AHEAD].dest = &track[83];
  track[81].edge[DIR_AHEAD].dist = 188;
  track[82].name = "BR2";
  track[82].type = NODE_BRANCH;
  track[82].num = 2;
  track[82].reverse = &track[83];
  track[82].edge[DIR_STRAIGHT].reverse = &track[81].edge[DIR_AHEAD];
  track[82].edge[DIR_STRAIGHT].src = &track[82];
  track[82].edge[DIR_STRAIGHT].dest = &track[80];
  track[82].edge[DIR_STRAIGHT].dist = 188;
  track[82].edge[DIR_CURVED].reverse = &track[7].edge[DIR_AHEAD];
  track[82].edge[DIR_CURVED].src = &track[82];
  track[82].edge[DIR_CURVED].dest = &track[6];
  track[82].edge[DIR_CURVED].dist = 229;
  track[83].name = "MR2";
  track[83].type = NODE_MERGE;
  track[83].num = 2;
  track[83].reverse = &track[82];
  track[83].edge[DIR_AHEAD].reverse = &track[84].edge[DIR_CURVED];
  track[83].edge[DIR_AHEAD].src = &track[83];
  track[83].edge[DIR_AHEAD].dest = &track[85];
  track[83].edge[DIR_AHEAD].dist = 185;
  track[84].name = "BR3";
  track[84].type = NODE_BRANCH;
  track[84].num = 3;
  track[84].reverse = &track[85];
  track[84].edge[DIR_STRAIGHT].reverse = &track[4].edge[DIR_AHEAD];
  track[84].edge[DIR_STRAIGHT].src = &track[84];
  track[84].edge[DIR_STRAIGHT].dest = &track[5];
  track[84].edge[DIR_STRAIGHT].dist = 231;
  track[84].edge[DIR_CURVED].reverse = &track[83].edge[DIR_AHEAD];
  track[84].edge[DIR_CURVED].src = &track[84];
  track[84].edge[DIR_CURVED].dest = &track[82];
  track[84].edge[DIR_CURVED].dist = 185;
  track[85].name = "MR3";
  track[85].type = NODE_MERGE;
  track[85].num = 3;
  track[85].reverse = &track[84];
  track[85].edge[DIR_AHEAD].reverse = &track[39].edge[DIR_AHEAD];
  track[85].edge[DIR_AHEAD].src = &track[85];
  track[85].edge[DIR_AHEAD].dest = &track[38];
  track[85].edge[DIR_AHEAD].dist = 128;
  track[86].name = "BR4";
  track[86].type = NODE_BRANCH;
  track[86].num = 4;
  track[86].reverse = &track[87];
  track[86].edge[DIR_STRAIGHT].reverse = &track[15].edge[DIR_AHEAD];
  track[86].edge[DIR_STRAIGHT].src = &track[86];
  track[86].edge[DIR_STRAIGHT].dest = &track[14];
  track[86].edge[DIR_STRAIGHT].dist = 417;
  track[86].edge[DIR_CURVED].reverse = &track[12].edge[DIR_AHEAD];
  track[86].edge[DIR_CURVED].src = &track[86];
  track[86].edge[DIR_CURVED].dest = &track[13];
  track[86].edge[DIR_CURVED].dist = 236;
  track[87].name = "MR4";
  track[87].type = NODE_MERGE;
  track[87].num = 4;
  track[87].reverse = &track[86];
  track[87].edge[DIR_AHEAD].reverse = &track[102].edge[DIR_CURVED];
  track[87].edge[DIR_AHEAD].src = &track[87];
  track[87].edge[DIR_AHEAD].dest = &track[103];
  track[87].edge[DIR_AHEAD].dist = 185;
  track[88].name = "BR5";
  track[88].type = NODE_BRANCH;
  track[88].num = 5;
  track[88].reverse = &track[89];
  track[88].edge[DIR_STRAIGHT].reverse = &track[35].edge[DIR_AHEAD];
  track[88].edge[DIR_STRAIGHT].src = &track[88];
  track[88].edge[DIR_STRAIGHT].dest = &track[34];
  track[88].edge[DIR_STRAIGHT].dist = 239;
  track[88].edge[DIR_CURVED].reverse = &track[92].edge[DIR_CURVED];
  track[88].edge[DIR_CURVED].src = &track[88];
  track[88].edge[DIR_CURVED].dest = &track[93];
  track[88].edge[DIR_CURVED].dist = 371;
  track[89].name = "MR5";
  track[89].type = NODE_MERGE;
  track[89].num = 5;
  track[89].reverse = &track[88];
  track[89].edge[DIR_AHEAD].reverse = &track[115].edge[DIR_AHEAD];
  track[89].edge[DIR_AHEAD].src = &track[89];
  track[89].edge[DIR_AHEAD].dest = &track[114];
  track[89].edge[DIR_AHEAD].dist = 155;
  track[90].name = "BR6";
  track[90].type = NODE_BRANCH;
  track[90].num = 6;
  track[90].reverse = &track[91];
  track[90].edge[DIR_STRAIGHT].reverse = &track[47].edge[DIR_AHEAD];
  track[90].edge[DIR_STRAIGHT].src = &track[90];
  track[90].edge[DIR_STRAIGHT].dest = &track[46];
  track[90].edge[DIR_STRAIGHT].dist = 239;
  track[90].edge[DIR_CURVED].reverse = &track[114].edge[DIR_CURVED];
  track[90].edge[DIR_CURVED].src = &track[90];
  track[90].edge[DIR_CURVED].dest = &track[115];
  track[90].edge[DIR_CURVED].dist = 371;
  track[91].name = "MR6";
  track[91].type = NODE_MERGE;
  track[91].num = 6;
  track[91].reverse = &track[90];
  track[91].edge[DIR_AHEAD].reverse = &track[36].edge[DIR_AHEAD];
  track[91].edge[DIR_AHEAD].src = &track[91];
  track[91].edge[DIR_AHEAD].dest = &track[37];
  track[91].edge[DIR_AHEAD].dist = 61;
  track[92].name = "BR7";
  track[92].type = NODE_BRANCH;
  track[92].num = 7;
  track[92].reverse = &track[93];
  track[92].edge[DIR_STRAIGHT].reverse = &track[59].edge[DIR_AHEAD];
  track[92].edge[DIR_STRAIGHT].src = &track[92];
  track[92].edge[DIR_STRAIGHT].dest = &track[58];
  track[92].edge[DIR_STRAIGHT].dist = 231;
  track[92].edge[DIR_CURVED].reverse = &track[88].edge[DIR_CURVED];
  track[92].edge[DIR_CURVED].src = &track[92];
  track[92].edge[DIR_CURVED].dest = &track[89];
  track[92].edge[DIR_CURVED].dist = 371;
  track[93].name = "MR7";
  track[93].type = NODE_MERGE;
  track[93].num = 7;
  track[93].reverse = &track[92];
  track[93].edge[DIR_AHEAD].reverse = &track[75].edge[DIR_AHEAD];
  track[93].edge[DIR_AHEAD].src = &track[93];
  track[93].edge[DIR_AHEAD].dest = &track[74];
  track[93].edge[DIR_AHEAD].dist = 50;
  track[94].name = "BR8";
  track[94].type = NODE_BRANCH;
  track[94].num = 8;
  track[94].reverse = &track[95];
  track[94].edge[DIR_STRAIGHT].reverse = &track[57].edge[DIR_AHEAD];
  track[94].edge[DIR_STRAIGHT].src = &track[94];
  track[94].edge[DIR_STRAIGHT].dest = &track[56];
  track[94].edge[DIR_STRAIGHT].dist = 316;
  track[94].edge[DIR_CURVED].reverse = &track[72].edge[DIR_AHEAD];
  track[94].edge[DIR_CURVED].src = &track[94];
  track[94].edge[DIR_CURVED].dest = &track[73];
  track[94].edge[DIR_CURVED].dist = 239;
  track[95].name = "MR8";
  track[95].type = NODE_MERGE;
  track[95].num = 8;
  track[95].reverse = &track[94];
  track[95].edge[DIR_AHEAD].reverse = &track[97].edge[DIR_AHEAD];
  track[95].edge[DIR_AHEAD].src = &track[95];
  track[95].edge[DIR_AHEAD].dest = &track[96];
  track[95].edge[DIR_AHEAD].dist = 155;
  track[96].name = "BR9";
  track[96].type = NODE_BRANCH;
  track[96].num = 9;
  track[96].reverse = &track[97];
  track[96].edge[DIR_STRAIGHT].reverse = &track[54].edge[DIR_AHEAD];
  track[96].edge[DIR_STRAIGHT].src = &track[96];
  track[96].edge[DIR_STRAIGHT].dest = &track[55];
  track[96].edge[DIR_STRAIGHT].dist = 309;
  track[96].edge[DIR_CURVED].reverse = &track[53].edge[DIR_AHEAD];
  track[96].edge[DIR_CURVED].src = &track[96];
  track[96].edge[DIR_CURVED].dest = &track[52];
  track[96].edge[DIR_CURVED].dist = 239;
  track[97].name = "MR9";
  track[97].type = NODE_MERGE;
  track[97].num = 9;
  track[97].reverse = &track[96];
  track[97].edge[DIR_AHEAD].reverse = &track[95].edge[DIR_AHEAD];
  track[97].edge[DIR_AHEAD].src = &track[97];
  track[97].edge[DIR_AHEAD].dest = &track[94];
  track[97].edge[DIR_AHEAD].dist = 155;
  track[98].name = "BR10";
  track[98].type = NODE_BRANCH;
  track[98].num = 10;
  track[98].reverse = &track[99];
  track[98].edge[DIR_STRAIGHT].reverse = &track[50].edge[DIR_AHEAD];
  track[98].edge[DIR_STRAIGHT].src = &track[98];
  track[98].edge[DIR_STRAIGHT].dest = &track[51];
  track[98].edge[DIR_STRAIGHT].dist = 239;
  track[98].edge[DIR_CURVED].reverse = &track[67].edge[DIR_AHEAD];
  track[98].edge[DIR_CURVED].src = &track[98];
  track[98].edge[DIR_CURVED].dest = &track[66];
  track[98].edge[DIR_CURVED].dist = 239;
  track[99].name = "MR10";
  track[99].type = NODE_MERGE;
  track[99].num = 10;
  track[99].reverse = &track[98];
  track[99].edge[DIR_AHEAD].reverse = &track[69].edge[DIR_AHEAD];
  track[99].edge[DIR_AHEAD].src = &track[99];
  track[99].edge[DIR_AHEAD].dest = &track[68];
  track[99].edge[DIR_AHEAD].dist = 50;
  track[100].name = "BR11";
  track[100].type = NODE_BRANCH;
  track[100].num = 11;
  track[100].reverse = &track[101];
  track[100].edge[DIR_STRAIGHT].reverse = &track[103].edge[DIR_AHEAD];
  track[100].edge[DIR_STRAIGHT].src = &track[100];
  track[100].edge[DIR_STRAIGHT].dest = &track[102];
  track[100].edge[DIR_STRAIGHT].dist = 188;
  track[100].edge[DIR_CURVED].reverse = &track[106].edge[DIR_STRAIGHT];
  track[100].edge[DIR_CURVED].src = &track[100];
  track[100].edge[DIR_CURVED].dest = &track[107];
  track[100].edge[DIR_CURVED].dist = 495;
  track[101].name = "MR11";
  track[101].type = NODE_MERGE;
  track[101].num = 11;
  track[101].reverse = &track[100];
  track[101].edge[DIR_AHEAD].reverse = &track[45].edge[DIR_AHEAD];
  track[101].edge[DIR_AHEAD].src = &track[101];
  track[101].edge[DIR_AHEAD].dest = &track[44];
  track[101].edge[DIR_AHEAD].dist = 43;
  track[102].name = "BR12";
  track[102].type = NODE_BRANCH;
  track[102].num = 12;
  track[102].reverse = &track[103];
  track[102].edge[DIR_STRAIGHT].reverse = &track[0].edge[DIR_AHEAD];
  track[102].edge[DIR_STRAIGHT].src = &track[102];
  track[102].edge[DIR_STRAIGHT].dest = &track[1];
  track[102].edge[DIR_STRAIGHT].dist = 231;
  track[102].edge[DIR_CURVED].reverse = &track[87].edge[DIR_AHEAD];
  track[102].edge[DIR_CURVED].src = &track[102];
  track[102].edge[DIR_CURVED].dest = &track[86];
  track[102].edge[DIR_CURVED].dist = 185;
  track[103].name = "MR12";
  track[103].type = NODE_MERGE;
  track[103].num = 12;
  track[103].reverse = &track[102];
  track[103].edge[DIR_AHEAD].reverse = &track[100].edge[DIR_STRAIGHT];
  track[103].edge[DIR_AHEAD].src = &track[103];
  track[103].edge[DIR_AHEAD].dest = &track[101];
  track[103].edge[DIR_AHEAD].dist = 188;
  track[104].name = "BR13";
  track[104].type = NODE_BRANCH;
  track[104].num = 13;
  track[104].reverse = &track[105];
  track[104].edge[DIR_STRAIGHT].reverse = &track[21].edge[DIR_AHEAD];
  track[104].edge[DIR_STRAIGHT].src = &track[104];
  track[104].edge[DIR_STRAIGHT].dest = &track[20];
  track[104].edge[DIR_STRAIGHT].dist = 231;
  track[104].edge[DIR_CURVED].reverse = &track[78].edge[DIR_AHEAD];
  track[104].edge[DIR_CURVED].src = &track[104];
  track[104].edge[DIR_CURVED].dest = &track[79];
  track[104].edge[DIR_CURVED].dist = 246;
  track[105].name = "MR13";
  track[105].type = NODE_MERGE;
  track[105].num = 13;
  track[105].reverse = &track[104];
  track[105].edge[DIR_AHEAD].reverse = &track[42].edge[DIR_AHEAD];
  track[105].edge[DIR_AHEAD].src = &track[105];
  track[105].edge[DIR_AHEAD].dest = &track[43];
  track[105].edge[DIR_AHEAD].dist = 120;
  track[106].name = "BR14";
  track[106].type = NODE_BRANCH;
  track[106].num = 14;
  track[106].reverse = &track[107];
  track[106].edge[DIR_STRAIGHT].reverse = &track[100].edge[DIR_CURVED];
  track[106].edge[DIR_STRAIGHT].src = &track[106];
  track[106].edge[DIR_STRAIGHT].dest = &track[101];
  track[106].edge[DIR_STRAIGHT].dist = 495;
  track[106].edge[DIR_CURVED].reverse = &track[43].edge[DIR_AHEAD];
  track[106].edge[DIR_CURVED].src = &track[106];
  track[106].edge[DIR_CURVED].dest = &track[42];
  track[106].edge[DIR_CURVED].dist = 333;
  track[107].name = "MR14";
  track[107].type = NODE_MERGE;
  track[107].num = 14;
  track[107].reverse = &track[106];
  track[107].edge[DIR_AHEAD].reverse = &track[2].edge[DIR_AHEAD];
  track[107].edge[DIR_AHEAD].src = &track[107];
  track[107].edge[DIR_AHEAD].dest = &track[3];
  track[107].edge[DIR_AHEAD].dist = 43;
  track[108].name = "BR15";
  track[108].type = NODE_BRANCH;
  track[108].num = 15;
  track[108].reverse = &track[109];
  track[108].edge[DIR_STRAIGHT].reverse = &track[37].edge[DIR_AHEAD];
  track[108].edge[DIR_STRAIGHT].src = &track[108];
  track[108].edge[DIR_STRAIGHT].dest = &track[36];
  track[108].edge[DIR_STRAIGHT].dist = 433;
  track[108].edge[DIR_CURVED].reverse = &track[40].edge[DIR_AHEAD];
  track[108].edge[DIR_CURVED].src = &track[108];
  track[108].edge[DIR_CURVED].dest = &track[41];
  track[108].edge[DIR_CURVED].dist = 326;
  track[109].name = "MR15";
  track[109].type = NODE_MERGE;
  track[109].num = 15;
  track[109].reverse = &track[108];
  track[109].edge[DIR_AHEAD].reverse = &track[31].edge[DIR_AHEAD];
  track[109].edge[DIR_AHEAD].src = &track[109];
  track[109].edge[DIR_AHEAD].dest = &track[30];
  track[109].edge[DIR_AHEAD].dist = 50;
  track[110].name = "BR16";
  track[110].type = NODE_BRANCH;
  track[110].num = 16;
  track[110].reverse = &track[111];
  track[110].edge[DIR_STRAIGHT].reverse = &track[17].edge[DIR_AHEAD];
  track[110].edge[DIR_STRAIGHT].src = &track[110];
  track[110].edge[DIR_STRAIGHT].dest = &track[16];
  track[110].edge[DIR_STRAIGHT].dist = 231;
  track[110].edge[DIR_CURVED].reverse = &track[19].edge[DIR_AHEAD];
  track[110].edge[DIR_CURVED].src = &track[110];
  track[110].edge[DIR_CURVED].dest = &track[18];
  track[110].edge[DIR_CURVED].dist = 239;
  track[111].name = "MR16";
  track[111].type = NODE_MERGE;
  track[111].num = 16;
  track[111].reverse = &track[110];
  track[111].edge[DIR_AHEAD].reverse = &track[41].edge[DIR_AHEAD];
  track[111].edge[DIR_AHEAD].src = &track[111];
  track[111].edge[DIR_AHEAD].dest = &track[40];
  track[111].edge[DIR_AHEAD].dist = 128;
  track[112].name = "BR17";
  track[112].type = NODE_BRANCH;
  track[112].num = 17;
  track[112].reverse = &track[113];
  track[112].edge[DIR_STRAIGHT].reverse = &track[61].edge[DIR_AHEAD];
  track[112].edge[DIR_STRAIGHT].src = &track[112];
  track[112].edge[DIR_STRAIGHT].dest = &track[60];
  track[112].edge[DIR_STRAIGHT].dist = 239;
  track[112].edge[DIR_CURVED].reverse = &track[63].edge[DIR_AHEAD];
  track[112].edge[DIR_CURVED].src = &track[112];
  track[112].edge[DIR_CURVED].dest = &track[62];
  track[112].edge[DIR_CURVED].dist = 246;
  track[113].name = "MR17";
  track[113].type = NODE_MERGE;
  track[113].num = 17;
  track[113].reverse = &track[112];
  track[113].edge[DIR_AHEAD].reverse = &track[76].edge[DIR_AHEAD];
  track[113].edge[DIR_AHEAD].src = &track[113];
  track[113].edge[DIR_AHEAD].dest = &track[77];
  track[113].edge[DIR_AHEAD].dist = 43;
  track[114].name = "BR18";
  track[114].type = NODE_BRANCH;
  track[114].num = 18;
  track[114].reverse = &track[115];
  track[114].edge[DIR_STRAIGHT].reverse = &track[38].edge[DIR_AHEAD];
  track[114].edge[DIR_STRAIGHT].src = &track[114];
  track[114].edge[DIR_STRAIGHT].dest = &track[39];
  track[114].edge[DIR_STRAIGHT].dist = 231;
  track[114].edge[DIR_CURVED].reverse = &track[90].edge[DIR_CURVED];
  track[114].edge[DIR_CURVED].src = &track[114];
  track[114].edge[DIR_CURVED].dest = &track[91];
  track[114].edge[DIR_CURVED].dist = 371;
  track[115].name = "MR18";
  track[115].type = NODE_MERGE;
  track[115].num = 18;
  track[115].reverse = &track[114];
  track[115].edge[DIR_AHEAD].reverse = &track[89].edge[DIR_AHEAD];
  track[115].edge[DIR_AHEAD].src = &track[115];
  track[115].edge[DIR_AHEAD].dest = &track[88];
  track[115].edge[DIR_AHEAD].dist = 155;
  track[116].name = "BR153";
  track[116].type = NODE_BRANCH;
  track[116].num = 153;
  track[116].reverse = &track[117];
  track[116].edge[DIR_STRAIGHT].reverse = &track[124].edge[DIR_AHEAD];
  track[116].edge[DIR_STRAIGHT].src = &track[116];
  track[116].edge[DIR_STRAIGHT].dest = &track[125];
  track[116].edge[DIR_STRAIGHT].dist = 253;
  track[116].edge[DIR_CURVED].reverse = &track[33].edge[DIR_AHEAD];
  track[116].edge[DIR_CURVED].src = &track[116];
  track[116].edge[DIR_CURVED].dest = &track[32];
  track[116].edge[DIR_CURVED].dist = 246;
  track[117].name = "MR153";
  track[117].type = NODE_MERGE;
  track[117].num = 153;
  track[117].reverse = &track[116];
  track[117].edge[DIR_AHEAD].reverse = &track[118].edge[DIR_STRAIGHT];
  track[117].edge[DIR_AHEAD].src = &track[117];
  track[117].edge[DIR_AHEAD].dest = &track[119];
  track[117].edge[DIR_AHEAD].dist = 0;
  track[118].name = "BR154";
  track[118].type = NODE_BRANCH;
  track[118].num = 154;
  track[118].reverse = &track[119];
  track[118].edge[DIR_STRAIGHT].reverse = &track[117].edge[DIR_AHEAD];
  track[118].edge[DIR_STRAIGHT].src = &track[118];
  track[118].edge[DIR_STRAIGHT].dest = &track[116];
  track[118].edge[DIR_STRAIGHT].dist = 0;
  track[118].edge[DIR_CURVED].reverse = &track[28].edge[DIR_AHEAD];
  track[118].edge[DIR_CURVED].src = &track[118];
  track[118].edge[DIR_CURVED].dest = &track[29];
  track[118].edge[DIR_CURVED].dist = 239;
  track[119].name = "MR154";
  track[119].type = NODE_MERGE;
  track[119].num = 154;
  track[119].reverse = &track[118];
  track[119].edge[DIR_AHEAD].reverse = &track[123].edge[DIR_AHEAD];
  track[119].edge[DIR_AHEAD].src = &track[119];
  track[119].edge[DIR_AHEAD].dest = &track[122];
  track[119].edge[DIR_AHEAD].dist = 0;
  track[120].name = "BR155";
  track[120].type = NODE_BRANCH;
  track[120].num = 155;
  track[120].reverse = &track[121];
  track[120].edge[DIR_STRAIGHT].reverse = &track[126].edge[DIR_AHEAD];
  track[120].edge[DIR_STRAIGHT].src = &track[120];
  track[120].edge[DIR_STRAIGHT].dest = &track[127];
  track[120].edge[DIR_STRAIGHT].dist = 282;
  track[120].edge[DIR_CURVED].reverse = &track[48].edge[DIR_AHEAD];
  track[120].edge[DIR_CURVED].src = &track[120];
  track[120].edge[DIR_CURVED].dest = &track[49];
  track[120].edge[DIR_CURVED].dist = 246;
  track[121].name = "MR155";
  track[121].type = NODE_MERGE;
  track[121].num = 155;
  track[121].reverse = &track[120];
  track[121].edge[DIR_AHEAD].reverse = &track[122].edge[DIR_STRAIGHT];
  track[121].edge[DIR_AHEAD].src = &track[121];
  track[121].edge[DIR_AHEAD].dest = &track[123];
  track[121].edge[DIR_AHEAD].dist = 0;
  track[122].name = "BR156";
  track[122].type = NODE_BRANCH;
  track[122].num = 156;
  track[122].reverse = &track[123];
  track[122].edge[DIR_STRAIGHT].reverse = &track[121].edge[DIR_AHEAD];
  track[122].edge[DIR_STRAIGHT].src = &track[122];
  track[122].edge[DIR_STRAIGHT].dest = &track[120];
  track[122].edge[DIR_STRAIGHT].dist = 0;
  track[122].edge[DIR_CURVED].reverse = &track[64].edge[DIR_AHEAD];
  track[122].edge[DIR_CURVED].src = &track[122];
  track[122].edge[DIR_CURVED].dest = &track[65];
  track[122].edge[DIR_CURVED].dist = 239;
  track[123].name = "MR156";
  track[123].type = NODE_MERGE;
  track[123].num = 156;
  track[123].reverse = &track[122];
  track[123].edge[DIR_AHEAD].reverse = &track[119].edge[DIR_AHEAD];
  track[123].edge[DIR_AHEAD].src = &track[123];
  track[123].edge[DIR_AHEAD].dest = &track[118];
  track[123].edge[DIR_AHEAD].dist = 0;
  track[124].name = "EN1";
  track[124].type = NODE_ENTER;
  track[124].reverse = &track[125];
  track[124].edge[DIR_AHEAD].reverse = &track[116].edge[DIR_STRAIGHT];
  track[124].edge[DIR_AHEAD].src = &track[124];
  track[124].edge[DIR_AHEAD].dest = &track[117];
  track[124].edge[DIR_AHEAD].dist = 253;
  track[125].name = "EX1";
  track[125].type = NODE_EXIT;
  track[125].reverse = &track[124];
  track[126].name = "EN2";
  track[126].type = NODE_ENTER;
  track[126].reverse = &track[127];
  track[126].edge[DIR_AHEAD].reverse = &track[120].edge[DIR_STRAIGHT];
  track[126].edge[DIR_AHEAD].src = &track[126];
  track[126].edge[DIR_AHEAD].dest = &track[121];
  track[126].edge[DIR_AHEAD].dist = 282;
  track[127].name = "EX2";
  track[127].type = NODE_EXIT;
  track[127].reverse = &track[126];
  track[128].name = "EN3";
  track[128].type = NODE_ENTER;
  track[128].reverse = &track[129];
  track[128].edge[DIR_AHEAD].reverse = &track[34].edge[DIR_AHEAD];
  track[128].edge[DIR_AHEAD].src = &track[128];
  track[128].edge[DIR_AHEAD].dest = &track[35];
  track[128].edge[DIR_AHEAD].dist = 514;
  track[129].name = "EX3";
  track[129].type = NODE_EXIT;
  track[129].reverse = &track[128];
  track[130].name = "EN4";
  track[130].type = NODE_ENTER;
  track[130].reverse = &track[131];
  track[130].edge[DIR_AHEAD].reverse = &track[13].edge[DIR_AHEAD];
  track[130].edge[DIR_AHEAD].src = &track[130];
  track[130].edge[DIR_AHEAD].dest = &track[12];
  track[130].edge[DIR_AHEAD].dist = 325;
  track[131].name = "EX4";
  track[131].type = NODE_EXIT;
  track[131].reverse = &track[130];
  track[132].name = "EN5";
  track[132].type = NODE_ENTER;
  track[132].reverse = &track[133];
  track[132].edge[DIR_AHEAD].reverse = &track[1].edge[DIR_AHEAD];
  track[132].edge[DIR_AHEAD].src = &track[132];
  track[132].edge[DIR_AHEAD].dest = &track[0];
  track[132].edge[DIR_AHEAD].dist = 504;
  track[133].name = "EX5";
  track[133].type = NODE_EXIT;
  track[133].reverse = &track[132];
  track[134].name = "EN6";
  track[134].type = NODE_ENTER;
  track[134].reverse = &track[135];
  track[134].edge[DIR_AHEAD].reverse = &track[14].edge[DIR_AHEAD];
  track[134].edge[DIR_AHEAD].src = &track[134];
  track[134].edge[DIR_AHEAD].dest = &track[15];
  track[134].edge[DIR_AHEAD].dist = 144;
  track[135].name = "EX6";
  track[135].type = NODE_EXIT;
  track[135].reverse = &track[134];
  track[136].name = "EN7";
  track[136].type = NODE_ENTER;
  track[136].reverse = &track[137];
  track[136].edge[DIR_AHEAD].reverse = &track[23].edge[DIR_AHEAD];
  track[136].edge[DIR_AHEAD].src = &track[136];
  track[136].edge[DIR_AHEAD].dest = &track[22];
  track[136].edge[DIR_AHEAD].dist = 43;
  track[137].name = "EX7";
  track[137].type = NODE_EXIT;
  track[137].reverse = &track[136];
  track[138].name = "EN8";
  track[138].type = NODE_ENTER;
  track[138].reverse = &track[139];
  track[138].edge[DIR_AHEAD].reverse = &track[11].edge[DIR_AHEAD];
  track[138].edge[DIR_AHEAD].src = &track[138];
  track[138].edge[DIR_AHEAD].dest = &track[10];
  track[138].edge[DIR_AHEAD].dist = 43;
  track[139].name = "EX8";
  track[139].type = NODE_EXIT;
  track[139].reverse = &track[138];
  track[140].name = "EN9";
  track[140].type = NODE_ENTER;
  track[140].reverse = &track[141];
  track[140].edge[DIR_AHEAD].reverse = &track[25].edge[DIR_AHEAD];
  track[140].edge[DIR_AHEAD].src = &track[140];
  track[140].edge[DIR_AHEAD].dest = &track[24];
  track[140].edge[DIR_AHEAD].dist = 50;
  track[141].name = "EX9";
  track[141].type = NODE_EXIT;
  track[141].reverse = &track[140];
  track[142].name = "EN10";
  track[142].type = NODE_ENTER;
  track[142].reverse = &track[143];
  track[142].edge[DIR_AHEAD].reverse = &track[27].edge[DIR_AHEAD];
  track[142].edge[DIR_AHEAD].src = &track[142];
  track[142].edge[DIR_AHEAD].dest = &track[26];
  track[142].edge[DIR_AHEAD].dist = 50;
  track[143].name = "EX10";
  track[143].type = NODE_EXIT;
  track[143].reverse = &track[142];

#ifndef TESTING
#pragma GCC diagnostic pop
#endif /* TESTING */

  for(int i = 0; i < NUM_TURNOUTS; i++) {
    turnouts[i] = TURNOUT_UNKNOWN; // offset 18-21 map to 153-156
  }
  for(unsigned int i = 0; i < 81; i++) {
    global_track->train[i].should_speed = 0;
  }

  default_value default_speeds[] = {
    {
      .train = 74,
      .speed = 1,
      .value = 927
    },
    {
      .train = 74,
      .speed = 2,
      .value = 7232
    },
    {
      .train = 74,
      .speed = 3,
      .value = 12487
    },
    {
      .train = 74,
      .speed = 4,
      .value = 17264
    },
    {
      .train = 74,
      .speed = 5,
      .value = 21792
    },
    {
      .train = 74,
      .speed = 6,
      .value = 26829
    },
    {
      .train = 74,
      .speed = 7,
      .value = 33375
    },
    {
      .train = 74,
      .speed = 8,
      .value = 36168
    },
    {
      .train = 74,
      .speed = 9,
      .value = 43879
    },
    {
      .train = 74,
      .speed = 10,
      .value = 47806
    },
    {
      .train = 74,
      .speed = 11,
      .value = 55389
    },
    {
      .train = 74,
      .speed = 13,
      .value = 61212
    },
    {
      .train = 74,
      .speed = 14,
      .value = 60798
    },
    { // Guard element
      .train = 1337,
      .speed = 1337,
      .value = 1337
    }
  };

  uint32_t super_default_speeds[15] = {
    0,
    927,
    7232,
    12487,
    17264,
    21792,
    26829,
    33375,
    36168,
    43879,
    47806,
    55389,
    56570,
    61212,
    60798
  };

  default_value default_stopping_distances[] = {
    { // Guard element
      .train = 1337,
      .speed = 1337,
      .value = 1337
    }
  };

  uint32_t super_default_stopping_distances[15] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  };

  default_value default_stopping_times[] = {
    { // Guard element
      .train = 1337,
      .speed = 1337,
      .value = 1337
    }
  };

  uint32_t super_default_stopping_times[15] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  };

  setup_default_map(global_track->speed_to_velocity, default_speeds, super_default_speeds);
  setup_default_map(global_track->stopping_distance, default_stopping_distances, super_default_stopping_distances);
  setup_default_map(global_track->stopping_time_mus, default_stopping_times, super_default_stopping_times);
}

unsigned int turnout_num_to_map_offset(unsigned int turnout) {
  if (turnout < 153) return turnout - 1;
  return turnout - (153 - 18);
}

unsigned int map_offset_to_turnout(unsigned int offset) {
  if (offset < 18) return offset + 1;
  return offset + (153 - 18);
}

bool is_valid_turnout_num(unsigned int turnout) {
  return (turnout >= 1 && turnout <= 18) || (turnout >= 153 && turnout <= 156);
}

char sensor_bank(unsigned int offset) {
  return 'A' + offset / 16;
}

unsigned int sensor_index(unsigned int offset) {
  return 1 + offset % 16;
}

unsigned int sensor_data_element(unsigned int offset) {
  return 2 * (sensor_bank(offset) - 'A') + (sensor_index(offset) >= 9 ? 1 : 0);
}

unsigned int sensor_data_mask(unsigned int offset) {
  return 1 << ((16 - sensor_index(offset)) % 8);
}

unsigned int sensor_offset(char bank, unsigned int index) {
  return 16 * (bank - 'A') + (index >= 9 ? 8 : 0) + (index - 1) % 8;
}

track_node *find_sensor(track_state *t, unsigned int offset) {
  char bank = sensor_bank(offset);
  int index = sensor_index(offset);

  char buf[4];
  buf[0] = bank;
  ui2a(index, 10, buf + 1);

  for (int i = 0; i < TRACK_MAX; i += 1) {
    track_node *current_node = &(t->track[i]);
    if (tstrncmp(buf, current_node->name, tstrlen(buf) + 1) == 0) {
      return current_node;
    }
  }

  Assert(0);
  return 0;
}

uint32_t distance_between_sensors_helper(track_node *start, track_node *end, uint32_t total_distance, int limit) {
  if (start == end) {
    return total_distance;
  } else if (limit == 0) {
    return 0;
  }

  switch (start->type) {
    case NODE_SENSOR:
    case NODE_MERGE: {
      track_node *next_node = start->edge[DIR_AHEAD].dest;
      uint32_t new_total_distance = total_distance + start->edge[DIR_AHEAD].dist;
      return distance_between_sensors_helper(next_node, end, new_total_distance, limit - 1);
    }
    case NODE_BRANCH: {
      track_node *straight_node = start->edge[DIR_STRAIGHT].dest;
      uint32_t straight_total_distance = total_distance + start->edge[DIR_STRAIGHT].dist;
      uint32_t result = distance_between_sensors_helper(straight_node, end, straight_total_distance, limit - 1);
      if (result != 0) {
        return result;
      }

      track_node *curved_node = start->edge[DIR_CURVED].dest;
      uint32_t curved_total_distance = total_distance + start->edge[DIR_CURVED].dist;
      return distance_between_sensors_helper(curved_node, end, curved_total_distance, limit - 1);
    }
    default:
      return 0;
  }
}

uint32_t distance_between_sensors(track_state *t, unsigned int start, unsigned int end) {
  if (start == end) return 0;
  track_node *start_node = find_sensor(t, start);
  track_node *end_node = find_sensor(t, end);
  int result = distance_between_sensors_helper(start_node, end_node, 0, 5);
  Assert(result != 0);
  return result;
}

uint32_t sensor_is_followed_by_helper(track_node *start, track_node *end, int limit) {
  if (start == end) {
    return true;
  } else if (limit == 0) {
    return false;
  }

  switch (start->type) {
    case NODE_MERGE:
      return sensor_is_followed_by_helper(start->edge[DIR_AHEAD].dest, end, limit - 1);
    case NODE_BRANCH: {
      track_node *straight_node = start->edge[DIR_STRAIGHT].dest;
      track_node *curved_node = start->edge[DIR_CURVED].dest;
      return sensor_is_followed_by_helper(straight_node, end, limit - 1) || sensor_is_followed_by_helper(curved_node, end, limit - 1);
    }
    default:
      return false;
  }
}

bool sensor_is_followed_by(track_state *t, unsigned int start, unsigned int end) {
  track_node *start_node = find_sensor(t, start);
  return start_node->type == NODE_SENSOR && sensor_is_followed_by_helper(start_node->edge[DIR_AHEAD].dest, find_sensor(t, end), 5);
}
