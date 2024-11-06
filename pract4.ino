#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>

#define MINPRESSURE 200
#define MAXPRESSURE 1000

// Pines para el touch (calibrados)
const int XP = 6, XM = A2, YP = A1, YM = 7;
const int TS_LEFT = 877, TS_RT = 149, TS_TOP = 926, TS_BOT = 134;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button plus_btn, minus_btn, left_btn, right_btn;

int pixel_x, pixel_y; // Coordenadas de toque
int boxSize = 50;     // Tamaño inicial del cuadro
int boxX = 240;       // Posición X del cuadro
int boxY = 120;       // Posición Y del cuadro
int rotationAngle = 0; // Ángulo de rotación del cuadro

bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT); // Restaurar los pines compartidos
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH); // Pines de control de TFT
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        // Asegurar que las coordenadas de pantalla estén mapeadas correctamente.
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
        
        // Para la rotación, invertir las coordenadas si la rotación cambia la orientación
        if (tft.getRotation() == 1 || tft.getRotation() == 3) {
            int temp = pixel_x;
            pixel_x = pixel_y;
            pixel_y = temp;
        }
    }
    return pressed;
}

#define BLACK 0x0000
#define WHITE 0xFFFF
#define CYAN  0x07FF

void drawBox() {
    // Limpiamos el área donde estaba el cuadro anterior
    tft.fillRect(180, 60, 120, 120, BLACK);

    // Dibujamos el nuevo cuadro con la rotación
    tft.setRotation(1);
    tft.drawRect(boxX - boxSize / 2, boxY - boxSize / 2, boxSize, boxSize, WHITE);
}

void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    if (ID == 0xD3D3) ID = 0x4532; // write-only shield
    tft.begin(ID);
    tft.setRotation(1); // Rotación en horizontal (landscape)
    tft.fillScreen(BLACK);

    // Inicializamos los botones en posiciones visibles
    plus_btn.initButton(&tft, 40, 80, 40, 40, WHITE, CYAN, BLACK, "+", 2);
    minus_btn.initButton(&tft, 110, 80, 40, 40, WHITE, CYAN, BLACK, "-", 2);
    left_btn.initButton(&tft, 40, 140, 40, 40, WHITE, CYAN, BLACK, "<-", 2);
    right_btn.initButton(&tft, 110, 140, 40, 40, WHITE, CYAN, BLACK, "->", 2);

    // Dibujamos los botones
    plus_btn.drawButton(false);
    minus_btn.drawButton(false);
    left_btn.drawButton(false);
    right_btn.drawButton(false);

    // Dibujamos el cuadro inicial
    drawBox();
}

void loop(void)
{
    bool down = Touch_getXY();

    // Actualizamos el estado de los botones
    plus_btn.press(down && plus_btn.contains(pixel_x, pixel_y));
    minus_btn.press(down && minus_btn.contains(pixel_x, pixel_y));
    left_btn.press(down && left_btn.contains(pixel_x, pixel_y));
    right_btn.press(down && right_btn.contains(pixel_x, pixel_y));

    // Acción al soltar los botones
    if (plus_btn.justReleased()) plus_btn.drawButton();
    if (minus_btn.justReleased()) minus_btn.drawButton();
    if (left_btn.justReleased()) left_btn.drawButton();
    if (right_btn.justReleased()) right_btn.drawButton();

    // Acción al presionar los botones
    if (plus_btn.justPressed()) {
        plus_btn.drawButton(true);
        boxSize += 10; // Aumenta el tamaño del cuadro
        if (boxSize > 120) boxSize = 120; // Límite superior
        drawBox();
    }
    if (minus_btn.justPressed()) {
        minus_btn.drawButton(true);
        boxSize -= 10; // Disminuye el tamaño del cuadro
        if (boxSize < 30) boxSize = 30; // Límite inferior
        drawBox();
    }
    if (left_btn.justPressed()) {
        left_btn.drawButton(true);
        rotationAngle -= 5; // Rotar a la izquierda
        drawBox();
    }
    if (right_btn.justPressed()) {
        right_btn.drawButton(true);
        rotationAngle += 5; // Rotar a la derecha
        drawBox();
    }
}