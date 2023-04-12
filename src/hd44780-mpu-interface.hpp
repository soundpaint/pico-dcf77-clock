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

#ifndef HD44780_MPU_INTERFACE_HPP
#define HD44780_MPU_INTERFACE_HPP

#include <cstdint>

class HD44780_mpu_interface
{
public:
  enum transfer_mode_t {
    TRANSFER_MODE_FOUR_BIT, TRANSFER_MODE_EIGHT_BIT
  };
  virtual ~HD44780_mpu_interface() {}
  virtual bool init() = 0;
  virtual transfer_mode_t
  select_transfer_mode(const transfer_mode_t preferred_transfer_mode) const = 0;
  virtual size_t exec_instruction(const uint8_t op_code) = 0;
  virtual size_t write_data(const uint8_t data) = 0;
  virtual bool supports_backlight_ctrl() const = 0;
  virtual bool set_backlight_enabled(const bool enable) = 0;
  virtual bool is_backlight_enabled() const = 0;
};

#endif /* HD44780_MPU_INTERFACE_HPP */

/*
 * Local variables:
 *   mode: c++
 *  coding: utf-8
 * End:
 */