# Flappy Bird on STM32 (Event-Driven)

## Giới thiệu

Dự án triển khai trò chơi **Flappy Bird** trên vi điều khiển **STM32**, sử dụng màn hình **OLED SSD1306** và kiến trúc **event-driven**. Trò chơi được thiết kế theo hướng không chặn (non-blocking), phù hợp với hệ thống nhúng có tài nguyên hạn chế.

---

## Phần cứng sử dụng

* Vi điều khiển: STM32l151 
* Màn hình OLED SSD1306 (I2C)
* Nút nhấn điều khiển (3 nut)
* Buzzer / LED 

---

## Kiến trúc hệ thống

* Event-driven
* State Machine
* Tách riêng các khối:

  * Xử lý logic game
  * Vẽ giao diện
  * Xử lý nút nhấn
  * Quản lý trạng thái

### Sơ đồ kiến trúc tổng thể

```
+-------------------+
|     Button ISR    |
+---------+---------+
          |
          v
+-------------------+        +-------------------+
|   Event Handler   | -----> |   State Machine   |
+---------+---------+        +---------+---------+
          |                            |
          v                            v
+-------------------+        +-------------------+
|   Game_Update()   | -----> |   Game_Draw()     |
+---------+---------+        +---------+---------+
          |                            |
          v                            v
+-------------------+        +-------------------+
|   Game Logic      |        |   OLED SSD1306    |
| (Physics, Score,  |        |      Display      |
|  Collision)       |        +-------------------+
+-------------------+
```

Sơ đồ trên thể hiện luồng xử lý chính của hệ thống, trong đó các sự kiện từ nút nhấn được đưa vào bộ xử lý sự kiện, sau đó cập nhật trạng thái trò chơi và logic game. Phần hiển thị được tách riêng và chỉ phụ thuộc vào trạng thái hiện tại, đảm bảo kiến trúc rõ ràng và dễ mở rộng.

---

## Các trạng thái game

* `GAME_WAIT_START`: Chờ người chơi bắt đầu
* `GAME_PLAYING`: Đang chơi
* `GAME_OVER`: Kết thúc trò chơi
* `GAME_HOME`: Menu chính
* `GAME_MENU`: Menu cài đặt
* `GAME_HISTORY`: Lịch sử điểm
* `GAME_RANK`: Bảng xếp hạng
* `GAME_BIRD_SELECT`: Chọn nhân vật
* `GAME_MENU_DIFFICULTY_SELECT,`: Chọn độ khó cho game
* `GAME_PLATFORM`: Màn hình nền 
  
---

## Các chức năng chính

### Khởi tạo game

* `Game_Init()`
* Thiết lập chim, ống cản, điểm số và trạng thái ban đầu

### Cập nhật logic

* `Game_Update()`
* Xử lý trọng lực, va chạm, tính điểm và chuyển trạng thái

### Vẽ giao diện

* `Game_Draw()`
* Hiển thị giao diện theo từng trạng thái game

### Quản lý ống cản

* `Pipes_ResetNormal()`
* Khởi tạo lại ống sau chế độ đặc biệt

### Lưu điểm và xếp hạng

* `SaveScoreToHistory()`
* `UpdateRanking()`

---

## Tính năng nổi bật

* Nhiều mức độ khó: Easy / Medium / Hard
* Trọng lực theo hành tinh: Mars / Earth / Jupiter
* Chế độ thưởng không có ống cản
* Mở khóa nhân vật bằng điểm thưởng (cups)
* Lưu lịch sử chơi và bảng xếp hạng

---

## Thư mục chính

```
Core/
 ├── Src/        // Source code
 ├── Inc/        // Header files
Drivers/         // Thư viện HAL
```

---

## Build & Flash

* Keil C
* Compiler: ARM-GCC

---



## Tác giả

* Hiếu Nguyễn

---
## Video demo 

Nguồn: https://drive.google.com/file/d/1MZCyKduyS832CezS1TOdv1B_aDJDYoFX/view?usp=sharing

## Bản quyền và ghi nhận

Dự án này được phát triển cho mục đích **học tập và nghiên cứu**.

Một số kit, thư viện và tài liệu được sử dụng trong dự án **không thuộc quyền sở hữu của tác giả**, bao gồm:

- Thư viện điều khiển màn hình OLED SSD1306  
  Nguồn: https://github.com/afiskon/stm32-ssd1306  
  Tác giả: Alexey Dynda  
  License: MIT

- Kit thực hành: AK Embedded Base Kit  
  Nguồn: https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu

