## Qt Basics Primer — before touching any UI file

---

### What is Qt?

Qt is a C++ framework for building desktop applications. It gives you:

- **Widgets** — buttons, windows, panels, plots (visual building blocks)
- **Signals & Slots** — Qt's way of connecting events to responses
- **Event loop** — the heartbeat that keeps the UI alive and responsive
- **Object ownership** — Qt manages memory for UI objects via a parent/child tree

---

### The single most important concept — Signals & Slots

This is Qt's answer to callbacks. Instead of raw function pointers, Qt uses a cleaner system:

```cpp
// Something happens → signal emitted
emit buttonClicked();

// Something reacts → slot called
void onButtonClicked() { ... }

// Wire them together
connect(button, &QPushButton::clicked,
        this,   &MyWindow::onButtonClicked);
```

Think of it like this:

- **Signal** = "something happened" announcement
- **Slot** = "I'll handle that" response
- **connect()** = the wire between them

This is exactly like your `out_pin` → `in_pin` connection in Sentinex — not a coincidence, the ideas are related.



### Object ownership in Qt

```cpp
auto* widget = new QWidget(parent);  // parent owns widget
// when parent is destroyed, widget is automatically destroyed too
```

Qt uses raw `new` with parent pointers — looks dangerous but Qt's ownership tree handles cleanup. You'll see this throughout the UI code.


### The event loop

```cpp
int main() {
    QApplication app(argc, argv);  // creates the Qt runtime
    MainWindow w;
    w.show();
    return app.exec();             // starts the event loop — blocks here
}                                  // until the window is closed
```

`app.exec()` is an infinite loop under the hood — it processes mouse clicks, keyboard input, timer ticks, window repaints, everything. Your `filter_graph::run()` is the equivalent for the simulation side.


### Q_OBJECT macro

You'll see this in every Qt class:

```cpp
class MainWindow : public QMainWindow
{
    Q_OBJECT   // ← this
    ...
};
```

This tells Qt's preprocessor (called moc) to generate the signal/slot wiring code behind the scenes. Every class that uses signals or slots must have it.


### QtNodes — the visual node editor

Your UI uses a third-party library called QtNodes on top of Qt. It provides:

- The draggable node boxes you see on screen
- The wires you draw between nodes
- Callbacks when nodes/connections are added or removed

Your code listens to those callbacks and mirrors every visual change into the `runtime_graph`.

---

That's all you need to understand the UI code. Let's go into `main.cpp` now — it's only 23 lines and is the perfect entry point.

---

In your pipeline, one `out_pin` can connect to **multiple** `in_pin`s (fan-out). You even store multiple receivers in a `std::map` inside `out_pin`.

**Do you think Qt signals work the same way — can one signal connect to multiple slots? Or is it one-to-one?**

Correct — Qt signals are also fan-out. One signal can connect to as many slots as you want. Exactly like your `out_pin` which stores multiple receivers in that `std::map`.

---

Next question — about the `Q_OBJECT` macro.

You saw this in the primer:

```cpp
class MainWindow : public QMainWindow
{
    Q_OBJECT
    ...
};
```

I told you Qt has a preprocessor called **moc** that reads this macro and generates code behind the scenes.

**Why do you think Qt needs a separate preprocessor to generate code, instead of just using normal C++ templates like your `out_pin<T>` does?**

Both `moc` and `runtime_graph` solve **runtime type identification** — knowing what type something is while the program is running, not just at compile time.



