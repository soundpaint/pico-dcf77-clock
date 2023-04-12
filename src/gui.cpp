/*
 * DCF77 Radio Clock GUI for RP2040
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

#include "gui.hpp"

#include <cstdio>
#include "hw061.hpp"

const uint16_t
GUI::LCD_WIDTH = 16; // characters per line

const uint16_t
GUI::LCD_LINES = 2; // number of lines

const HD44780::pattern_size_t
GUI::LCD_DOTSIZE = HD44780::FONT_5x8;

const uint8_t
GUI::ICON_BELL[] = {
  0b00000,
  0b00100,
  0b01110,
  0b01110,
  0b01110,
  0b11111,
  0b00100,
  0b00000,
};

const uint8_t
GUI::ICON_PHONE[] = {
  0b00000,
  0b01110,
  0b10101,
  0b01110,
  0b11011,
  0b10001,
  0b11111,
  0b00000,
};

const uint8_t
GUI::ICON_TRIANGLE_RIGHT[] = {
  0b00000,
  0b01000,
  0b01100,
  0b01110,
  0b01111,
  0b01110,
  0b01100,
  0b01000,
};

const uint8_t
GUI::ICON_SIGNAL_LOW[] = {
  0b00000,
  0b00000,
  0b00000,
  0000000,
  0b00001,
  0b00000,
  0b00000,
  0b00000,
};

const uint8_t
GUI::ICON_SIGNAL_MEDIUM[] = {
  0b00000,
  0b00000,
  0b00011,
  0000100,
  0b00101,
  0b00100,
  0b00011,
  0b00000,
};

const uint8_t
GUI::ICON_SIGNAL_HIGH[] = {
  0b00111,
  0b01000,
  0b10011,
  0b10100,
  0b10101,
  0b10100,
  0b10011,
  0b01000,
};

const char *
GUI::STR_DAY_OF_THE_WEEK[] = {
  "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa",
};

GUI::GUI()
{
  lcd_mpu_interface = 0;
  lcd = 0;
}

GUI::~GUI()
{
  delete lcd;
  lcd = 0;
  delete lcd_mpu_interface;
  lcd_mpu_interface = 0;
}

bool
GUI::init()
{
  sleep_ms(300); // ensure LCD has powered up
  lcd_mpu_interface = new HW061();
  if (lcd_mpu_interface) {
    if (lcd_mpu_interface->init()) {
      lcd = new HD44780(lcd_mpu_interface);
      sleep_ms(100);
      lcd->set_character_pattern(0, ICON_BELL);
      lcd->set_character_pattern(1, ICON_PHONE);
      lcd->set_character_pattern(2, ICON_TRIANGLE_RIGHT);
      lcd->set_character_pattern(3, ICON_SIGNAL_LOW);
      lcd->set_character_pattern(4, ICON_SIGNAL_MEDIUM);
      lcd->set_character_pattern(5, ICON_SIGNAL_HIGH);
      lcd->set_backlight_enabled(true);
      if (lcd) {
        lcd->reset_display_control();
        lcd->clear_display();
        lcd->set_cursor_position(0, 0);
        lcd->write("    J\xF5rgen's    ");
        lcd->set_cursor_position(0, 1);
        lcd->write(" DCF77 Receiver ");
      }
    } else {
      delete lcd_mpu_interface;
      lcd_mpu_interface = 0;
      lcd = 0;
    }
  } else {
    lcd = 0;
  }
  if (!lcd) {
    printf("warning: failed initalizing LCD\n");
  }
  return lcd != 0;
}

void
GUI::deinit()
{
  delete lcd;
}

void
GUI::clear()
{
  lcd->clear_display();
}

void
GUI::update(const datetime_t datetime, const bool dst, const bool call_bit)
{
  char buffer[32];
  // do *not* clear LCD to avoid flickering
  const char *timezone = dst ? "MESZ" : "MEZ ";
  std::snprintf(buffer, sizeof(buffer) - 1, "%02d:%02d:%02d %s",
                datetime.hour, datetime.min, datetime.sec, timezone);
  lcd->set_cursor_position(0, 0);
  lcd->write(buffer);
  const char *str_call = call_bit ? "\x00" : "\x01";
  std::snprintf(buffer, sizeof(buffer) - 1, "%s, %02d.%02d.%04d %s",
                STR_DAY_OF_THE_WEEK[datetime.dotw],
                datetime.day, datetime.month, datetime.year, str_call);
  lcd->set_cursor_position(0, 1);
  lcd->write(buffer);
}

/*
 * Local variables:
 *   mode: c++
 *  coding: utf-8
 * End:
 */
