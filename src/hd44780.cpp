/*
 * HD44780 Driver for RP2040
 *
 * Copyright (C) 2023 Jürgen Reuter
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * As a special exception to the GNU General Public License, if you
 * distribute this file as part of a program that contains a
 * configuration script generated by Autoconf, you may include it
 * under the same distribution terms that you use for the rest of that
 * program.
 *
 * For updates and more info or contacting the author, visit:
 * <https://github.com/soundpaint>
 *
 * Author's web site: www.juergen-reuter.de
 */

#include "hd44780.hpp"

#include <cstring>
#include "pico/stdlib.h"

HD44780::HD44780(HD44780_mpu_interface *hd44780_mpu_interface,
                 const screen_width_t screen_width,
                 const screen_height_t screen_height,
                 const pattern_size_t pattern_size,
                 const HD44780_mpu_interface::transfer_mode_t
                 preferred_transfer_mode)
{
  assert(hd44780_mpu_interface);
  HD44780::hd44780_mpu_interface = hd44780_mpu_interface;
  init(screen_width, screen_height, pattern_size, preferred_transfer_mode);
}

HD44780::~HD44780()
{
  hd44780_mpu_interface = 0;
}

void
HD44780::init(const screen_width_t screen_width,
              const screen_height_t screen_height,
              const pattern_size_t pattern_size,
              const HD44780_mpu_interface::transfer_mode_t
              preferred_transfer_mode)
{
  const HD44780_mpu_interface::transfer_mode_t transfer_mode =
    hd44780_mpu_interface->select_transfer_mode(preferred_transfer_mode);
  _function_set =
    transfer_mode == HD44780_mpu_interface::TRANSFER_MODE_EIGHT_BIT ?
    0x10 : 0x00;

  switch (screen_height) {
  case LINES_1:
    _number_of_lines = 1;
    break;
  case LINES_2:
    _function_set |= 0x08;
    _number_of_lines = 2;
    break;
  case LINES_4:
    _function_set |= 0x08;
    _number_of_lines = 4;
    break;
  }

  _line_start_addr[0] = 0x00;
  _line_start_addr[1] = 0x40;
  switch (screen_width) {
  case COLUMNS_16:
    _line_start_addr[2] = 0x10;
    _line_start_addr[3] = 0x50;
    break;
  default:
    _line_start_addr[2] = 0x14;
    _line_start_addr[3] = 0x54;
    break;
  }

  // support for 10x5 font available only for single line displays
  if ((pattern_size == FONT_5x10) && (screen_height == LINES_1)) {
    _function_set |= 0x04;
  }

  sleep_us(50000);

  /*
   * For details on initialization for 4 bit transfer mode vs. 8 bit
   * transfer mode, see Sect. "Initilialization by Instruction",
   * pp. 45-46, Hitachi HD44780U datasheet.
   */
  if (transfer_mode == HD44780_mpu_interface::TRANSFER_MODE_FOUR_BIT) {
    exec_instruction(0x03);
    sleep_us(4500);
    exec_instruction(0x03);
    sleep_us(4500);
    exec_instruction(0x03);
    sleep_us(150);
    exec_instruction(0x02);
  } else {
    exec_function_set();
    sleep_us(4500);
    exec_function_set();
    sleep_us(150);
    exec_function_set();
  }

  exec_function_set();
  reset_display_control();
  clear_display();

  _entry_mode_set = 0x00;
  set_text_direction(LEFT_TO_RIGHT);
}

bool
HD44780::supports_backlight_ctrl() const
{
  return hd44780_mpu_interface->supports_backlight_ctrl();
}

bool
HD44780::set_backlight_enabled(const bool enable)
{
  return hd44780_mpu_interface->set_backlight_enabled(enable);
}

bool
HD44780::is_backlight_enabled() const
{
  return hd44780_mpu_interface->is_backlight_enabled();
}

/*
 * Clear display.  Set cursor address to zero.
 */
void
HD44780::clear_display()
{
  exec_instruction(0x01);
  sleep_us(2000);
}

/*
 * Set cursor address to 0.  Set display shift to 0.
 */
void
HD44780::return_home()
{
  exec_instruction(0x02);
  sleep_us(2000);
}

void
HD44780::set_cursor_position(const uint8_t column, uint8_t line)
{
  const uint8_t line_clipped =
    line > _number_of_lines - 1 ? _number_of_lines - 1 : line;
  exec_instruction(0x80 | (column + _line_start_addr[line_clipped]));
}

/*
 * Turn on display.  Turn off cursor blinking.  Turn off cursor.
 */
void
HD44780::reset_display_control()
{
  _display_control = 0x04;
  set_display_enabled(true);
}

void
HD44780::shift(const direction_t direction, const bool cursor_only)
{
  const uint8_t DIRECTION_MASK = direction == LEFT_TO_RIGHT ? 0x04 : 0x00;
  const uint8_t SC_MASK = cursor_only ? 0x00: 0x08;
  exec_instruction(0x10 | SC_MASK | DIRECTION_MASK);
}

void
HD44780::exec_entry_mode_set()
{
  exec_instruction(0x04 | _entry_mode_set);
}

void
HD44780::set_text_direction(const direction_t direction)
{
  const uint8_t INC_DEC_MASK = 0x02;
  switch (direction) {
  case LEFT_TO_RIGHT:
    _entry_mode_set |= INC_DEC_MASK;
    break;
  case RIGHT_TO_LEFT:
    _entry_mode_set &= ~INC_DEC_MASK;
    break;
  }
  exec_entry_mode_set();
}

void
HD44780::set_auto_scroll_enabled(const bool enable)
{
  const uint8_t DISPLAY_SHIFT_MASK = 0x01;
  if (enable) {
    _entry_mode_set |= DISPLAY_SHIFT_MASK;
  } else {
    _entry_mode_set &= ~DISPLAY_SHIFT_MASK;
  }
  exec_entry_mode_set();
}

void
HD44780::exec_function_set()
{
  exec_instruction(0x20 | _function_set);
}

void
HD44780::exec_display_control()
{
  exec_instruction(0x08 | _display_control);
}

void
HD44780::set_display_enabled(const bool enable)
{
  const uint8_t DISPLAY_MASK = 0x04;
  if (enable) {
    _display_control |= DISPLAY_MASK;
  } else {
    _display_control &= ~DISPLAY_MASK;
  }
  exec_display_control();
}

void
HD44780::set_cursor_enabled(const bool enable)
{
  const uint8_t CURSOR_MASK = 0x02;
  if (enable) {
    _display_control |= CURSOR_MASK;
  } else {
    _display_control &= ~CURSOR_MASK;
  }
  exec_display_control();
}

void
HD44780::set_cursor_blinking_enabled(const bool enable)
{
  const uint8_t BLINKING_MASK = 0x01;
  if (enable) {
    _display_control |= BLINKING_MASK;
  } else {
    _display_control &= ~BLINKING_MASK;
  }
  exec_display_control();
}

void
HD44780::set_character_pattern(const uint8_t character_code,
                               const uint8_t pattern[])
{
  exec_instruction(0x40 | (character_code << 3));
  for (int line = 0; line < 8; line++) {
    write_data(pattern[line]);
  }
}

inline void
HD44780::exec_instruction(const uint8_t op_code)
{
  hd44780_mpu_interface->exec_instruction(op_code);
}

size_t
HD44780::write(const char *buffer)
{
  if (!buffer) return 0;
  return write((const uint8_t *)buffer, strlen(buffer));
}

size_t
HD44780::write(const uint8_t *buffer, const size_t size)
{
  size_t remaining = size;
  size_t written = 0;
  while (remaining--) {
    if (write_data(*buffer++)) {
      written++;
    } else {
      break;
    }
  }
  return written;
}

inline size_t
HD44780::write_data(const uint8_t data)
{
  hd44780_mpu_interface->write_data(data);
  return 1;
}

/*
 * Local variables:
 *   mode: c++
 *  coding: utf-8
 * End:
 */
