# Issue 02: Match Exit Confirmation Modal

Status: closed
Type: task

## Description

Implement `MatchExitPrompt` modal:
1. Rendered when user initiates exit from a multiplayer match.
2. Displays clear warning: "FORFEIT MATCH? Leaving will concede victory to your opponent."
3. Primary button: "FORFEIT & LEAVE" (red danger button calling `gameSocket.leaveRoom()`).
4. Secondary button: "CANCEL" (dismisses prompt and resumes play).
