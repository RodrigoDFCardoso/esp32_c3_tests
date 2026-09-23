from machine import Pin, SPI
import time


# ============================================================
# ESP32-C3 SUPER MINI + ST7789 240x240
# ============================================================

PIN_SCK  = 4       # ST7789 SCL/SCK
PIN_MOSI = 6       # ST7789 SDA/MOSI
PIN_RST  = 21      # ST7789 RST / GPIO21 (TX)
PIN_DC   = 9       # ST7789 DC / GPIO9 (SCL)
PIN_CS   = 8       # ST7789 CS / GPIO8 (SDA)


# ============================================================
# CORES RGB565
# ============================================================

BLACK   = 0x0000
WHITE   = 0xFFFF
RED     = 0xF800
GREEN   = 0x07E0
BLUE    = 0x001F
YELLOW  = 0xFFE0
CYAN    = 0x07FF
MAGENTA = 0xF81F


# ============================================================
# COMANDOS ST7789
# ============================================================

SWRESET = 0x01
SLPOUT  = 0x11
COLMOD  = 0x3A
MADCTL  = 0x36
CASET   = 0x2A
RASET   = 0x2B
RAMWR   = 0x2C
INVON   = 0x21
DISPON  = 0x29


# ============================================================
# DRIVER ST7789
# ============================================================

class ST7789:

    def __init__(self):

        self.width = 240
        self.height = 240

        # SPI hardware da ESP32-C3
        self.spi = SPI(
            1,
            baudrate=40_000_000,
            polarity=0,
            phase=0,
            sck=Pin(PIN_SCK),
            mosi=Pin(PIN_MOSI)
        )

        # GPIOs definidos manualmente
        self.dc = Pin(PIN_DC, Pin.OUT)
        self.rst = Pin(PIN_RST, Pin.OUT)
        self.cs = Pin(PIN_CS, Pin.OUT)

        # Estado inicial
        self.cs.value(1)
        self.dc.value(1)

        # Inicialização
        self.reset()
        self.init_display()

    # ========================================================
    # RESET
    # ========================================================

    def reset(self):

        self.rst.value(1)
        time.sleep_ms(50)

        self.rst.value(0)
        time.sleep_ms(100)

        self.rst.value(1)
        time.sleep_ms(150)

    # ========================================================
    # ENVIA COMANDO
    # ========================================================

    def command(self, cmd):

        self.cs.value(0)

        self.dc.value(0)

        self.spi.write(bytes([cmd]))

        self.cs.value(1)

    # ========================================================
    # ENVIA DADOS
    # ========================================================

    def data(self, data):

        self.cs.value(0)

        self.dc.value(1)

        self.spi.write(data)

        self.cs.value(1)

    # ========================================================
    # INICIALIZAÇÃO DO ST7789
    # ========================================================

    def init_display(self):

        # Software reset
        self.command(SWRESET)
        time.sleep_ms(150)

        # Sai do modo sleep
        self.command(SLPOUT)
        time.sleep_ms(120)

        # 16 bits por pixel / RGB565
        self.command(COLMOD)
        self.data(b'\x55')
        time.sleep_ms(10)

        # Orientação
        self.command(MADCTL)
        self.data(b'\x00')

        # Área inicial
        self.set_window(
            0,
            0,
            self.width - 1,
            self.height - 1
        )

        # Inversão
        self.command(INVON)

        time.sleep_ms(10)

        # Display ON
        self.command(DISPON)

        time.sleep_ms(100)

    # ========================================================
    # DEFINE ÁREA DE ESCRITA
    # ========================================================

    def set_window(self, x0, y0, x1, y1):

        # Colunas
        self.command(CASET)

        self.data(bytes([
            x0 >> 8,
            x0 & 0xFF,
            x1 >> 8,
            x1 & 0xFF
        ]))

        # Linhas
        self.command(RASET)

        self.data(bytes([
            y0 >> 8,
            y0 & 0xFF,
            y1 >> 8,
            y1 & 0xFF
        ]))

        # Inicia escrita
        self.command(RAMWR)

    # ========================================================
    # PREENCHE TELA
    # ========================================================

    def fill(self, color):

        hi = color >> 8
        lo = color & 0xFF

        # Buffer de 256 pixels
        buffer = bytes([hi, lo]) * 256

        self.set_window(
            0,
            0,
            self.width - 1,
            self.height - 1
        )

        self.cs.value(0)
        self.dc.value(1)

        total = self.width * self.height

        blocks = total // 256
        remainder = total % 256

        for _ in range(blocks):
            self.spi.write(buffer)

        if remainder:
            self.spi.write(
                bytes([hi, lo]) * remainder
            )

        self.cs.value(1)

    # ========================================================
    # PIXEL
    # ========================================================

    def pixel(self, x, y, color):

        if x < 0 or x >= self.width:
            return

        if y < 0 or y >= self.height:
            return

        self.set_window(x, y, x, y)

        self.cs.value(0)
        self.dc.value(1)

        self.spi.write(bytes([
            color >> 8,
            color & 0xFF
        ]))

        self.cs.value(1)

    # ========================================================
    # RETÂNGULO
    # ========================================================

    def rect(self, x, y, w, h, color):

        if w <= 0 or h <= 0:
            return

        x2 = x + w - 1
        y2 = y + h - 1

        self.set_window(x, y, x2, y2)

        hi = color >> 8
        lo = color & 0xFF

        buffer = bytes([hi, lo]) * 256

        total = w * h

        self.cs.value(0)
        self.dc.value(1)

        blocks = total // 256
        remainder = total % 256

        for _ in range(blocks):
            self.spi.write(buffer)

        if remainder:
            self.spi.write(
                bytes([hi, lo]) * remainder
            )

        self.cs.value(1)

    # ========================================================
    # LINHA HORIZONTAL
    # ========================================================

    def hline(self, x, y, w, color):
        self.rect(x, y, w, 1, color)

    # ========================================================
    # LINHA VERTICAL
    # ========================================================

    def vline(self, x, y, h, color):
        self.rect(x, y, 1, h, color)


# ============================================================
# INICIALIZAÇÃO
# ============================================================

print()
print("====================================")
print(" ESP32-C3 + ST7789 240x240")
print("====================================")
print("SCK  :", PIN_SCK)
print("MOSI :", PIN_MOSI)
print("RST  :", PIN_RST)
print("DC   :", PIN_DC)
print("CS   :", PIN_CS)
print("====================================")

display = ST7789()

print("ST7789 inicializado!")


# ============================================================
# TESTE DE CORES
# ============================================================

display.fill(BLACK)
time.sleep(1)

display.fill(RED)
time.sleep(1)

display.fill(GREEN)
time.sleep(1)

display.fill(BLUE)
time.sleep(1)

display.fill(WHITE)
time.sleep(1)


# ============================================================
# TELA DE TESTE
# ============================================================

display.fill(BLACK)

# Borda superior
display.hline(0, 0, 240, WHITE)

# Borda inferior
display.hline(0, 239, 240, WHITE)

# Borda esquerda
display.vline(0, 0, 240, WHITE)

# Borda direita
display.vline(239, 0, 240, WHITE)


# Quadrados superiores
display.rect(20, 20, 60, 60, RED)
display.rect(90, 20, 60, 60, GREEN)
display.rect(160, 20, 60, 60, BLUE)

# Quadrados inferiores
display.rect(20, 100, 60, 60, YELLOW)
display.rect(90, 100, 60, 60, CYAN)
display.rect(160, 100, 60, 60, MAGENTA)

print("Teste concluído.")