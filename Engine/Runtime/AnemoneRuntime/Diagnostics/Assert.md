# Assertions

Assertions are used to check for conditions that should never happen. They are used to catch bugs early in development.

Configurations with assertions enabled always evaluate and check the condition. Depending on the macro, the program may crash or continue execution.

| Macro     | Description                            |
|-----------|----------------------------------------|
| AE_ASSERT | Compiled out in shipping builds.       |
| AE_VERIFY | Condition is always evaluated.         |
| AE_ENSURE | Checked in shipping builds.            |
| AE_PANIC  | Crashes the program in shipping build. |
