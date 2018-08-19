/*
* gisound.cpp
*
* Copyright (C) 2005 Mark Broadhead
*
* This file is part of DAPHNE, a laserdisc arcade game emulator
*
* DAPHNE is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* DAPHNE is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef GISOUND_H
#define GISOUND_H

#include <stdint.h>

int gisound_initialize(uint32_t core_frequency);
void gisound_writedata(uint32_t, uint32_t, int index);
void gisound_stream(uint8_t* stream, int length, int index);
void gisound_shutdown(int index);

enum {
   CHANNEL_A_TONE_PERIOD_FINE,
   CHANNEL_A_TONE_PERIOD_COARSE,
   CHANNEL_B_TONE_PERIOD_FINE,
   CHANNEL_B_TONE_PERIOD_COARSE,
   CHANNEL_C_TONE_PERIOD_FINE,
   CHANNEL_C_TONE_PERIOD_COARSE,
   NOISE_PERIOD,
   ENABLE,
   CHANNEL_A_AMPLITUDE,
   CHANNEL_B_AMPLITUDE,
   CHANNEL_C_AMPLITUDE,
   ENVELOPE_PERIOD_FINE,
   ENVELOPE_PERIOD_COARSE,
   ENVELOPE_SHAPE_CYCLE,
   IO_PORT_A_DATA_STORE,
   IO_PORT_B_DATA_STORE,
};

#define TONE_A_ENABLE 0 
#define TONE_B_ENABLE 1 
#define TONE_C_ENABLE 2 


struct gi_sound_chip;

struct gi_sound_chip {
   // Properties
   uint32_t core_clock;

   // Registers
   uint8_t register_set[16];

   int chan_a_bytes_per_switch;
   int chan_b_bytes_per_switch;
   int chan_c_bytes_per_switch;
   int chan_a_bytes_to_go;
   int chan_b_bytes_to_go;
   int chan_c_bytes_to_go;
   uint8_t noise_period;
   int noise_bytes_per_switch;
   int noise_bytes_to_go;
   int noise_flip;
   bool iob_in;
   bool ioa_in;
   bool noise_c;
   bool noise_b;
   bool noise_a;
   bool tone_c;
   bool tone_b;
   bool tone_a;
   uint8_t chan_a_amplitude;
   int chan_a_flip;
   bool chan_a_amplitude_mode;
   uint8_t chan_b_amplitude;
   int chan_b_flip;
   bool chan_b_amplitude_mode;
   uint8_t chan_c_amplitude;
   int chan_c_flip;
   bool chan_c_amplitude_mode;
   int envelope_period;
   bool envelope_cycle_complete;
   uint8_t envelope_amplitude;
   int envelope_bytes_to_go;
   uint8_t envelope_step;
   bool envelope_shape_cycle_cont;
   bool envelope_shape_cycle_att;
   bool envelope_shape_cycle_alt;
   bool envelope_shape_cycle_hold;
   uint8_t port_a_data_store;
   uint8_t port_b_data_store;
   uint32_t random_seed;
};

#endif
