/*
 * An example of integrating TaskManager events with LiquidCrystalIO. Showing how the event handling can
 * be used to perform screen refreshes while other events continue to run. We compiled this for Dfrobot shield
 * but could be easily adapted for other combinations.
 */
#include <TaskManagerIO.h>
#include <LiquidCrystalIO.h>

// This example initialises for DfRobot, it's the simplest case in LiquidCrystalIO and requires no parameters, because
// we know upfront what the pin configuration is. See other examples for how to provide pin parameters.
LiquidCrystal lcd;

/**
 * Here we create an event that handles all the drawing for an application, in this case printing out readings
 * of a sensor when changed. It uses polling and immediate triggering to show both examples
 */
class DrawingEvent : public BaseEvent {
private:
    int heaterTemperature;
    bool heaterIsOn;
    volatile bool emergency; // if an event comes from an external interrupt the variable must be volatile.
    bool hasChanged;
public:
    /**
     * This is called by task manager every time the number of microseconds returned expires, if you trigger the
     * event it will run the exec(), if you complete the event, it will be removed from task manager.
     * @return the number of micros before calling again.
     */
    uint32_t timeOfNextCheck() override {
        setTriggered(hasChanged);
        return millisToMicros(500); // no point refreshing more often on an LCD, as its unreadable
    }

    /**
     * This is called when the event is triggered, it prints all the data onto the screen.
     */
    void exec() override {
        hasChanged = false;

        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print(heaterTemperature);

        lcd.setCursor(10, 1);
        lcd.print(heaterIsOn ? " ON" : "OFF");

        lcd.setCursor(14, 1);
        lcd.print(emergency ? "!!" : "  ");
    }

    /**
     * This sets the latest temperature and heater status, but only marks the event changed, so it will need
     * to poll in order to trigger. This prevents excessive screen updates.
     * @param temp the new temperature
     * @param on if the heater is on
     */
    void setLatestStatus(int temp, bool on) {
        heaterTemperature = temp;
        heaterIsOn = on;
        hasChanged = true;// we are happy to wait out the 500 millis
    }

    /**
     * Triggers an emergency that requires immediate update of the screen
     * @param isEmergency if there is an urgent notification
     */
    void triggerEmergency(bool isEmergency) {
        emergency = isEmergency;
        markTriggeredAndNotify(); // get on screen asap.
    }
};

// create an instance of the above class
DrawingEvent drawingEvent;

void setup() {
    lcd.begin(16, 2);
    lcd.setCursor(0,0);
    lcd.print("Heater:");
    lcd.setCursor(0,1);
    lcd.print("Temp:");

    // we create a watchdog task that simulates reading a sensor state and updating the drawing event.
    taskManager.scheduleFixedRate(50, [] {
        // this simulates a reading from your system that needs to be displayed, we run this in it's own task
        int temp = (rand() % 200) - 100;
        bool isOn = rand() % 2;
        drawingEvent.setLatestStatus(temp, isOn);
    }, TIME_MILLIS);

    // here we create a couple of tasks that represent triggering and clearing an emergency.
    taskManager.scheduleOnce(10, [] { drawingEvent.triggerEmergency(true);}, TIME_SECONDS);
    taskManager.scheduleOnce(30, [] { drawingEvent.triggerEmergency(false);}, TIME_SECONDS);

    // create any other tasks that you need here for your sketch

    taskManager.registerEvent(&drawingEvent);
}

void loop() {
    taskManager.runLoop();
}