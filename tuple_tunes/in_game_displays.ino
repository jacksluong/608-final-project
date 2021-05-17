/////////////
// In-game //
/////////////

void display_in_game() {
  tft.fillRect(0, 0, 106, 128, TFT_BLACK);
  tft.fillRect(106, 0, 80, 128, rgb_to_565(DARK_GRAY));
  
  // Grid
  tft.setCursor(5, 6, 1);
  tft.printf("<measure %d/%d>", selected_measure, MEASURE_COUNT);
  tft.fillRect(0, 0, 106, 128, TFT_BLACK);
  tft.fillRect(106, 0, 80, 128, rgb_to_565(DARK_GRAY));

  // Left side
  for (int i = 0; i < 4; i++) tft.drawRect(0, 25 * i + 19, 106, 25, TFT_WHITE);
  display_measure(selected_measure);

  // Right side
  menu_state = in_turn ? 0 : 4;

  
  if (in_turn) {
    draw_text("Add Note", 110, 60, CYAN, 1);
    draw_text("Submit", 115, 80, CYAN, 1);
  }

  for (int i = 0; i < 3; i++) { // ellipses
    tft.fillCircle(4 * i + 146, 119, 1, TFT_WHITE);
  }

  update_in_game(0, false);
}

void update_in_game(int js, bool note_added) {
  // Clearing
  if (is_locked) {
    
    if (menu_state == 0) tft.fillRect(125, 15, 25, 15, rgb_to_565(DARK_GRAY)); // selected note
    else if (menu_state == 1) tft.fillRect(122, 40, 25, 15, rgb_to_565(DARK_GRAY)); // duration
    else if (menu_state == 4) tft.fillRect(119, 100, 30, 15, rgb_to_565(DARK_GRAY)); // measure
    tft.drawTriangle(115, 21 + 20 * menu_state, 
                     110, 23 + 20 * menu_state,
                     115, 25 + 20 * menu_state, TFT_WHITE);
    tft.drawTriangle(151, 21 + 20 * menu_state, 
                     156, 23 + 20 * menu_state,
                     151, 25 + 20 * menu_state, TFT_WHITE); // arrows for note selection
  } else {
    tft.drawTriangle(115,21,110,23,115,25, rgb_to_565(DARK_GRAY)); // arrows for note selection
    tft.drawTriangle(151,21,156,23,151,25, rgb_to_565(DARK_GRAY));
    tft.drawTriangle(115,41,110,43,115,45, rgb_to_565(DARK_GRAY)); // arrows for duration selection
    tft.drawTriangle(151,41,156,43,151,45, rgb_to_565(DARK_GRAY));
    tft.drawTriangle(115,101,110,103,115,105, rgb_to_565(DARK_GRAY)); // arrows for measure selection
    tft.drawTriangle(151,101,156,103,151,105, rgb_to_565(DARK_GRAY));
    if (menu_state == 0 || menu_state == 1 || menu_state == 4) {
      tft.drawTriangle(115, 21 + 20 * menu_state, 
                       110, 23 + 20 * menu_state,
                       115, 25 + 20 * menu_state, rgb_to_565(GRAY));
      tft.drawTriangle(151, 21 + 20 * menu_state, 
                       156, 23 + 20 * menu_state,
                       151, 25 + 20 * menu_state, rgb_to_565(GRAY)); // arrows for current menu state
    }
  }
  if (js == 1 || js == 3 || note_added) // menu cursor
    for (int i = 0; i < 5; i++) tft.fillCircle(134, 30 + 20 * i, 1, rgb_to_565(DARK_GRAY)); 
  if (note_added) { // grid cursor
    int current_x = 8 + 25 * (note_state % 4);
    int current_y = 38 + 25 * (int(note_state / 4));
    tft.drawLine(current_x, current_y, current_x + 10, current_y, TFT_BLACK);
  }

  if (in_turn) {
    tft.setCursor(132, 20, 1); 
    tft.println(current_note); // print current selected note
    if (selected_duration == 0) { // 1/16
      tft.setCursor(122, 40, 1); 
    } else if (selected_duration == 4) { // 1
      tft.setCursor(131, 40, 1); 
    } else {
      tft.setCursor(125, 40, 1);
    }
    tft.println(NOTE_DURATIONS[selected_duration]); // print current selected note duration
  }
  if (current_measure < 10) { // scroll through measures
    tft.setCursor(122, 100, 1);
  } else {
    tft.setCursor(117, 100, 1); 
  }
  tft.printf("m. %d", selected_measure + 1);
  
  // Left cursor
  if (note_state < 16) {
    set_cursor_pos(2 + 25 * (note_state % 4), 29 + 25*(int(note_state/4)));
    draw_cursor();
  }
  
  // Right cursor
  tft.fillCircle(134, 30 + 20 * menu_state, 1, TFT_WHITE);

  // Update selected note
  if (is_locked && menu_state == 0) draw_note(note_state, measures[current_measure][note_state]);
  if (note_added) display_measure(current_measure);

  Serial.printf("Current/selected measure: %d/%d, note_state (c_n_i): %d (%d)\n", current_measure, selected_measure, note_state, curr_note_index);
}

void display_measure(int measure_i) {
  tft.fillRect(4, 5, 75, 13, TFT_BLACK);
  tft.setTextColor(rgb_to_565(GRAY));
  tft.setCursor(5, 6, 1);
  tft.printf("<measure %d/%d>", selected_measure + 1, MEASURE_COUNT);
  for (int i = 0; i < (measure_i == current_measure ? min(note_state + 1, 16) : 16); i++)
    draw_note(i, measures[measure_i][i]);
}

void draw_note(int note_i, int note_num) {
  tft.fillRect(3 + 25 * (note_i % 4), 22 + 25 * (note_i / 4), 19, 21, TFT_BLACK);
  tft.setCursor(10 + 25 * (note_i % 4), 28 + 25 * (note_i / 4), 1);
  tft.setTextColor((selected_measure == current_measure && note_i == note_state) ? rgb_to_565(CYAN) : rgb_to_565(DARK_CYAN));
  if (note_num == 37) tft.println("~");
  else if (note_num == 36) tft.println("R");
  else tft.println((is_flat_key ? NOTES_FLAT : NOTES_SHARP)[note_num % 12]);
}

///////////////
// Game Menu //
///////////////

void display_game_menu() {
  tft.fillScreen(TFT_BLACK);
  
  // dividing line between left and right
  tft.drawLine(84, 0, 84, 128, TFT_WHITE);
  tft.drawLine(85, 0, 85, 128, TFT_WHITE);

  // left side actionables
  tft.setCursor(2, 6, 1);
  tft.println("Menu");
  tft.setCursor(4, 20, 1);
  tft.println(" Resume");
  tft.setCursor(4, 40, 1);
  if (!playing_song) {
    tft.println(" Play Song");
  } else {
    tft.println(" Stop Song");
  }
  tft.setCursor(4, 60, 1);
  if (!playing_measure) {
    tft.println(" Play Measure");
  } else {
    tft.println(" Stop Measure");
  }
  tft.setCursor(4, 80, 1);
  if (sound_on) {
    tft.println(" Mute");
  } else {
    tft.println(" Unmute");
  }
  tft.setCursor(4, 100, 1);
  tft.println(" Leave Game");

  // right side static
  tft.setCursor(88, 6, 1);
  tft.println("Game Info");
  tft.setCursor(88, 20, 1);
  tft.printf(" Room#:%s", room_num);
  tft.setCursor(88, 40, 1);
  tft.printf(" Key:%s", NOTES_FLAT[selected_key]);
  tft.setCursor(88, 60, 1);
  tft.printf(" Tempo:%s", TEMPO_LABELS[selected_tempo]);
  tft.setCursor(88, 80, 1);
  tft.printf(" #Players:%d", num_players);
  tft.setCursor(88, 100, 1);
  if (sound_on) {
    tft.println(" Sound On");
  } else {
    tft.println(" Sound off");
  }

  update_game_menu();
}

void update_game_menu() {
  tft.fillRect(2, 20, 4, 100, TFT_BLACK);
  set_cursor_pos(2, 21 + 20 * (menu_state % 5));
  draw_cursor();

  tft.fillRect(6, 78, 50, 22, TFT_BLACK);
  tft.setCursor(4, 80, 1);
  if (sound_on) {
    tft.println(" Mute");
  } else {
    tft.println(" Unmute");
  }

  tft.fillRect(88, 98, 70, 22, TFT_BLACK);
  tft.setCursor(88, 100, 1);
  if (sound_on) {
    tft.println(" Sound On");
  } else {
    tft.println(" Sound Off");
  }
}

//////////////
// End Game //
//////////////

void display_end_game() {
  tft.fillScreen(TFT_BLACK);
  display_in_game();
  tft.fillRect(106, 0, 80, 128, rgb_to_565(DARK_GRAY));
  draw_text("Play", 123, 10, CYAN, 1);
  draw_text("Song", 123, 20, CYAN, 1);
  draw_text("Play", 123, 40, CYAN, 1);
  draw_text("Measure", 113, 50, CYAN, 1);
  draw_text("New", 127, 70, CYAN, 1);
  draw_text("Game", 123, 80, CYAN, 1);

  if (current_measure < 10) { // scroll through measures
    tft.setCursor(122, 100, 1);
  } else {
    tft.setCursor(117, 100, 1); 
  }
  tft.printf("m. %d", selected_measure + 1);

  for (int i = 0; i < 3; i++) { // ellipses
    tft.fillCircle(4 * i + 146, 119, 1, TFT_WHITE);
  }
  
  update_end_game();
}

void update_end_game() {
  // Clearing
  if (is_locked) {
    if (menu_state == 3) tft.fillRect(119, 110, 20, 10, rgb_to_565(DARK_GRAY)); // measure
    if (menu_state == 3) {
      tft.drawTriangle(115, 11 + 30 * menu_state, 
                       110, 13 + 30 * menu_state,
                       115, 15 + 30 * menu_state, TFT_WHITE);
      tft.drawTriangle(151, 11 + 30 * menu_state, 
                       156, 13 + 30 * menu_state,
                       151, 15 + 30 * menu_state, TFT_WHITE); // arrows for measure selection
    }
  } else {
    tft.drawTriangle(115,121,110,103,115,105, rgb_to_565(DARK_GRAY)); // arrows for measure selection
    tft.drawTriangle(151,121,156,103,151,105, rgb_to_565(DARK_GRAY));
    if (menu_state == 3) {
      tft.drawTriangle(115, 11 + 30 * menu_state, 
                       110, 13 + 30 * menu_state,
                       115, 15 + 30 * menu_state, rgb_to_565(GRAY));
      tft.drawTriangle(151, 11 + 30 * menu_state, 
                       156, 13 + 30 * menu_state,
                       151, 15 + 30 * menu_state, rgb_to_565(GRAY)); // arrows for current menu state
    }
  }

  // Menu cursor
  if (menu_state == 3) {
    tft.fillCircle(134, 50 + 30 * (menu_state - 1), 1, TFT_WHITE);
  } else {
    tft.fillCircle(134, 30 + 30 * menu_state, 1, TFT_WHITE);
  }
}
