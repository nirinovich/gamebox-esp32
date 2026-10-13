# Issue 01: Mobile Ergonomics and Touch Controls Fixes

Status: closed

## Description
Fix the critical layout bug in the virtual D-Pad CSS Grid, increase touch target dimensions to 60px with tactile active press feedback, and implement direct canvas gesture listeners for touch devices (vertical paddle dragging in Pong, and cardinal swipe gestures in Tron and Snake).

## Acceptance Criteria
1. Virtual D-pad `.dpad-down` is positioned at `grid-column: 2; grid-row: 3` so it sits directly below the center instead of occupying the center row.
2. D-pad buttons are resized to at least 60px with `transform: scale(0.94)` active press state.
3. Pong canvas accepts direct vertical pointer drag events to move the player paddle smoothly 1:1.
4. Canvas accepts pointer swipe gestures (with 15px threshold) to steer in Tron and Snake without needing the D-pad buttons.
