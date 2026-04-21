# Contributing to Signal Analysis System

Thanks for your interest in contributing! 🎉

## How to Contribute

### 🐛 Reporting Bugs

Open an issue with:
- Arduino board model (Uno, Nano, Mega...)
- Library versions
- What you expected vs what happened
- Serial monitor output if applicable

### 💡 Suggesting Features

Open an issue titled `[Feature] Your idea here` and describe:
- What problem it solves
- How you imagine it working

### 🔧 Submitting Code

1. Fork the repository
2. Create a branch: `git checkout -b feature/your-feature`
3. Follow the code style (see below)
4. Test on real hardware before submitting
5. Open a Pull Request with a clear description

## Code Style

- Use `#define` for all pin and timing constants
- All timing must use `millis()` — no `delay()` calls
- New signal states go in the `STATES[]` array only
- Comment non-obvious logic
- Keep functions single-purpose and named clearly

## Testing Checklist

Before submitting a PR, verify on hardware:
- [ ] Boot screen displays correctly
- [ ] Button tap toggles ON/OFF
- [ ] Button hold (2s) resets to State 0
- [ ] All 7 states cycle correctly
- [ ] LEDs and buzzer match the alert level table
- [ ] No visual glitches on OLED during state transitions
- [ ] Serial output shows correct state changes
