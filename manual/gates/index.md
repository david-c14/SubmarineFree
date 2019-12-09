# Logic Gates

#### Conventions used in the truth tables

Across the top, the truth table shows the inputs, internal states, and outputs of the gate in that order. 

| | |
|-|-|
| A | An input, typically for a gate where the inputs are interchangeable |
| B | An input, typically for a gate where the inputs are interchangeable |
| C | An input, typically for a gate where the inputs are interchangeable, or an input representing a clock |
| D | An input. Either the 4th input where the inputs are interchangeable, or an input carrying data |
| E | An *Enable* input. Typically if a gate has an *Enable* input, its state will not change unless *E* is high |
| R | A *Reset* input. Representing a signal to reset the output to 0 (false) |
| S | A *Set* input. Representing a signal to set the output to 1 (true) |
| Q | The output of the logic gate |
| 0 | A value of 0, logical false |
| 1 | A value of 1, logical true |
| &#x2715; | Either a 0 or a 1, false or true |
| N&#x305; | A line over a symbol indicates the logical inverse of that value |
| &#x1f809; | An upwards arrow indicates that the gate responds to a rising edge. Where the input goes from 0 to 1 |
| &#x1f80b; | A downwards arrow indicates that the gate responds to a falling edge. Where the input goes from 1 to 0 |
| N&#x2080; | A tiny 0 after a value indicates the value in the *previous* sample |
| N&#x2081; | A tiny 1 after a value indicates the value in *this* sample | 
 
### N/C

When no gate is selected, the DO-1xx device shows N/C which is short for Not Connected. The output is always 0 (false)

### NOT gate

The NOT gate has a single input A. The output Q is always the logical inverse of A. So when A is 1 (true) then Q is 0 (false);
and when A is 0 (false) then Q is 1 (true)

| A | Q |
|---|---|
| 0 | 1 |
| 1 | 0 | 

![timing diagram for NOT gate](NOT_timing.png "NOT")

### AND gate

| A | B | Q |
|---|---|---|
| 0 | 0 | 0 |
| 0 | 1 | 0 |
| 1 | 0 | 0 |
| 1 | 1 | 1 |

![timing diagram for AND gate](2-AND_timing.png "AND")

### 3-input AND gate

| A | B | C | Q |
|---|---|---|---|
| 0 | 0 | 0 | 0 |
| 0 | 0 | 1 | 0 |
| 0 | 1 | 0 | 0 |
| 0 | 1 | 1 | 0 |
| 1 | 0 | 0 | 0 |
| 1 | 0 | 1 | 0 |
| 1 | 1 | 0 | 0 |
| 1 | 1 | 1 | 1 |

![timing diagram for 3-input AND gate](3-AND_timing.png "3-input AND")

### 4-input AND gate

| A | B | C | D | Q |
|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 0 |
| 0 | 0 | 0 | 1 | 0 |
| 0 | 0 | 1 | 0 | 0 |
| 0 | 0 | 1 | 1 | 0 |
| 0 | 1 | 0 | 0 | 0 |
| 0 | 1 | 0 | 1 | 0 |
| 0 | 1 | 1 | 0 | 0 |
| 0 | 1 | 1 | 1 | 0 |
| 1 | 0 | 0 | 0 | 0 |
| 1 | 0 | 0 | 1 | 0 |
| 1 | 0 | 1 | 0 | 0 |
| 1 | 0 | 1 | 1 | 0 |
| 1 | 1 | 0 | 0 | 0 |
| 1 | 1 | 0 | 1 | 0 |
| 1 | 1 | 1 | 0 | 0 |
| 1 | 1 | 1 | 1 | 1 |

![timing diagram for 4-input AND gate](4-AND_timing.png "4-input AND")

### NAND gate

| A | B | Q |
|---|---|---|
| 0 | 0 | 1 |
| 0 | 1 | 1 |
| 1 | 0 | 1 |
| 1 | 1 | 0 |

![timing diagram for NAND gate](2-NAND_timing.png "NAND")

## 3-input NAND gate

| A | B | C | Q |
|---|---|---|---|
| 0 | 0 | 0 | 1 |
| 0 | 0 | 1 | 1 |
| 0 | 1 | 0 | 1 |
| 0 | 1 | 1 | 1 |
| 1 | 0 | 0 | 1 |
| 1 | 0 | 1 | 1 |
| 1 | 1 | 0 | 1 |
| 1 | 1 | 1 | 0 |

![timing diagram for 3-input NAND gate](3-NAND_timing.png "3-input NAND")

### 4-input NAND gate

| A | B | C | D | Q |
|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 1 |
| 0 | 0 | 0 | 1 | 1 |
| 0 | 0 | 1 | 0 | 1 |
| 0 | 0 | 1 | 1 | 1 |
| 0 | 1 | 0 | 0 | 1 |
| 0 | 1 | 0 | 1 | 1 |
| 0 | 1 | 1 | 0 | 1 |
| 0 | 1 | 1 | 1 | 1 |
| 1 | 0 | 0 | 0 | 1 |
| 1 | 0 | 0 | 1 | 1 |
| 1 | 0 | 1 | 0 | 1 |
| 1 | 0 | 1 | 1 | 1 |
| 1 | 1 | 0 | 0 | 1 |
| 1 | 1 | 0 | 1 | 1 |
| 1 | 1 | 1 | 0 | 1 |
| 1 | 1 | 1 | 1 | 0 |

![timing diagram for 4-input NAND gate](4-NAND_timing.png "4-input NAND")

### OR gate

| A | B | C | D | Q |
|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 0 |
| 0 | 0 | 0 | 1 | 1 |
| 0 | 0 | 1 | 0 | 1 |
| 0 | 0 | 1 | 1 | 1 |
| 0 | 1 | 0 | 0 | 1 |
| 0 | 1 | 0 | 1 | 1 |
| 0 | 1 | 1 | 0 | 1 |
| 0 | 1 | 1 | 1 | 1 |
| 1 | 0 | 0 | 0 | 1 |
| 1 | 0 | 0 | 1 | 1 |
| 1 | 0 | 1 | 0 | 1 |
| 1 | 0 | 1 | 1 | 1 |
| 1 | 1 | 0 | 0 | 1 |
| 1 | 1 | 0 | 1 | 1 |
| 1 | 1 | 1 | 0 | 1 |
| 1 | 1 | 1 | 1 | 1 |

![timing diagram for OR gate](OR_timing.png "OR")

### NOR gate

| A | B | C | D | Q |
|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 1 |
| 0 | 0 | 0 | 1 | 0 |
| 0 | 0 | 1 | 0 | 0 |
| 0 | 0 | 1 | 1 | 0 |
| 0 | 1 | 0 | 0 | 0 |
| 0 | 1 | 0 | 1 | 0 |
| 0 | 1 | 1 | 0 | 0 |
| 0 | 1 | 1 | 1 | 0 |
| 1 | 0 | 0 | 0 | 0 |
| 1 | 0 | 0 | 1 | 0 |
| 1 | 0 | 1 | 0 | 0 |
| 1 | 0 | 1 | 1 | 0 |
| 1 | 1 | 0 | 0 | 0 |
| 1 | 1 | 0 | 1 | 0 |
| 1 | 1 | 1 | 0 | 0 |
| 1 | 1 | 1 | 1 | 0 |

![timing diagram for NOR gate](NOR_timing.png "NOR")

### XOR gate

| A | B | C | D | Q |
|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 0 |
| 0 | 0 | 0 | 1 | 1 |
| 0 | 0 | 1 | 0 | 1 |
| 0 | 0 | 1 | 1 | 0 |
| 0 | 1 | 0 | 0 | 1 |
| 0 | 1 | 0 | 1 | 0 |
| 0 | 1 | 1 | 0 | 0 |
| 0 | 1 | 1 | 1 | 1 |
| 1 | 0 | 0 | 0 | 1 |
| 1 | 0 | 0 | 1 | 0 |
| 1 | 0 | 1 | 0 | 0 |
| 1 | 0 | 1 | 1 | 1 |
| 1 | 1 | 0 | 0 | 0 |
| 1 | 1 | 0 | 1 | 1 |
| 1 | 1 | 1 | 0 | 1 |
| 1 | 1 | 1 | 1 | 0 |

![timing diagram for XOR gate](XOR_timing.png "XOR")

### XNOR gate

| A | B | C | D | Q |
|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 1 |
| 0 | 0 | 0 | 1 | 0 |
| 0 | 0 | 1 | 0 | 0 |
| 0 | 0 | 1 | 1 | 1 |
| 0 | 1 | 0 | 0 | 0 |
| 0 | 1 | 0 | 1 | 1 |
| 0 | 1 | 1 | 0 | 1 |
| 0 | 1 | 1 | 1 | 0 |
| 1 | 0 | 0 | 0 | 0 |
| 1 | 0 | 0 | 1 | 1 |
| 1 | 0 | 1 | 0 | 1 |
| 1 | 0 | 1 | 1 | 0 |
| 1 | 1 | 0 | 0 | 1 |
| 1 | 1 | 0 | 1 | 0 |
| 1 | 1 | 1 | 0 | 0 |
| 1 | 1 | 1 | 1 | 1 |

![timing diagram for XNOR gate](XNOR_timing.png "XNOR")

### D-TYPE-FLIPFLOP

| C&#x2081; | Q&#x2081; |
|---|---|
| C&#x2080; | Q&#x2080; |
| &#x1f80b; | Q&#x2080; |
| &#x1f809; | Q&#x305;&#x2080; |

![timing diagram for D-TYPE-FLIPFLOP](D-TYPE-FLIPFLOP_timing.png "D-TYPE-FLIPFLOP")

### SR-FLIPFLOP

| S | R | Q&#x2081; |
|---|---|---|
| 0 | 0 | Q&#x2080; |
| 0 | 1 | 0 |
| 1 | &#x2715; | 1 |

![timing diagram for SR-FLIPFLOP](SR-FLIPFLOP_timing.png "SR-FLIPFLOP")

### D-TYPE-LATCH

| D | E | Q&#x2081; |
|---|---|---|
| &#x2715; | 0 | Q&#x2080; |
| 0 | 1 | 0 |
| 1 | 1 | 1 |

![timing diagram for D-TYPE-LATCH](D-TYPE-LATCH_timing.png "D-TYPE-LATCH")

### SR-LATCH

| S | R | E&#x2081; | Q&#x2081; |
|---|---|---|---|
| &#x2715; | &#x2715; | E&#x2080; | Q&#x2080; |
| &#x2715; | &#x2715; | &#x1f80b; | Q&#x2080; |
| 0 | 0 | &#x1f809; | Q&#x2080; |
| 0 | 1 | &#x1f809; | 0 |
| 1 | &#x2715; | &#x1f809; | 1 |

![timing diagram for SR-LATCH](SR-LATCH_timing.png "SR-LATCH")

### LATCH

| D | E&#x2081; | Q&#x2081; |
|---|---|---|
| &#x2715; | E&#x2080; | Q&#x2080; |
| &#x2715; | &#x1f80b; | Q&#x2080; |
| 0 | &#x1f809; | 0 |
| 1 | &#x1f809; | 1 |

![timing diagram for LATCH](LATCH_timing.png "LATCH")

### DELAY

| D&#x2081; | Q |
|---|---|
| &#x2715; | D&#x2080; |

![timing_diagram_for_DELAY](DELAY_timing.png "DELAY")

### CLK-DELAY

| C&#x2081; | D | A&#x2081; | Q&#x2081; |
|---|---|---|---|
| C&#x2080; | &#x2715; | A&#x2080; | Q&#x2080; |
| &#x1f80b; | &#x2715; | A&#x2080; | Q&#x2080; |
| &#x1f809; | 0 | 0 | A&#x2080; |
| &#x1F809; | 1 | 1 | A&#x2080; |

![timing diagram for CLK-DELAY](CLK-DELAY_timing.png "CLK-DELAY")
