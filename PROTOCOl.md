# CppChat Protocol

## 1. Message Framing
We use Length-Prefixing.
- **Format:** `[Message Length (as text)]:[Message Content]`
- **Example:** To send "Hi", the actual network data will be `2:Hi`.
- **Example:** To send "Hello", the data will be `5:Hello`.

## 2. Message Types
After the length prefix, the message content will start with a command, followed by a colon, then the data.

- **Regular Chat:** `MSG:<text>`
  - Example: `13:MSG:Hello all!`
  
- **Set Nickname:** `NICK:<name>`
  - Example: `9:NICK:Wolf`
  
- **Disconnect:** `QUIT:`
  - Example: `5:QUIT:`
