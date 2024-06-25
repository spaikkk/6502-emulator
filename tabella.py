def isIt4(carattere):
    stringa = "289abce"
    if (carattere in stringa):
        return 1
    else:
        return 0

def isItA(carattere):
    stringa="1357df"
    if (carattere in stringa):
        return 0
    else:
        return 1

def isItC(carattere):
    stringa = "2468abce"
    if (carattere in stringa):
        return 1
    else:
        return 0

def okNotOK(opCode):
    stringa = hex(opCode)
    daControllare = stringa[2:]
    if (len(daControllare)==1):
        hexString = '0'+daControllare
    else:
        hexString=daControllare
    strCompleti = "1568d"
    strIncompleti = "37bf"
    if (hexString[1] in strCompleti):
        return 1
    elif (hexString[1] in strIncompleti):
        return 0
    elif hexString[1] == '0':
        if hexString[0] == '8':
            return 0 
        else:
            return 1
    elif hexString[1]=='2':
        if hexString[0]=='a':
            return 1
        else:
            return 0
    elif hexString[1]=='4':
        return isIt4(hexString[0])
    elif hexString[1]=='9':
        if hexString[0]=='8':
            return 0 
        else: 
            return 1
    elif hexString[1]=='a':
        return isItA(hexString[0])
    elif hexString[1]=='c':
        return isItC(hexString[0])
    elif hexString[1]=='e':
        if hexString[0]=='9':
            return 0
        else:
            return 1
        
        


file1 = open("pitoDump.c", "w")
intro = """
#include "functions.h"

int funzione(opCode){
"""
file1.write(intro)
file1.write("    switch (opCode) {\n")
for givenOpCode in range(255):
    c = givenOpCode & 3
    b = (givenOpCode & 0b00011100) >> 2
    a = (givenOpCode & 0b11100000) >> 5
    if b == 0:
        if c == 1:
            addressMode = "X_INDEXED_INDIRECT"
        elif c == 0 and a < 4:
            if a == 1:
                addressMode = "ABSOLUTE"
            else:
                addressMode = "IMPLIED"
        else:
            addressMode = "IMMEDIATE"
    elif b == 1:
        addressMode = "ZEROPAGE"
    elif b == 2:
        if c == 0 or (c == 2 and a >= 4):
            addressMode = "IMPLIED"
        elif c == 1:
            addressMode = "IMMEDIATE"
        elif c == 2 and a < 4:
            addressMode = "ACCUMULATOR"
    elif b == 3:
        if givenOpCode == 0x6C:
            addressMode = "INDIRECT"
        else:
            addressMode = "ABSOLUTE"
    elif b == 4:
        if c == 1:
            addressMode = "INDIRECT_Y_INDEXED"
        else:
            addressMode = "RELATIVE"
    elif b == 5:
        if (a == 4 and c == 2) or (a == 5 and c == 2):
            addressMode = "ZEROPAGE_Y_INDEXED"
        else:
            addressMode = "ZEROPAGE_X_INDEXED"
    elif b == 6:
        if c == 0:
            addressMode = "IMPLIED"
        elif c == 1:
            addressMode = "ABSOLUTE_Y"
        else:
            addressMode = "IMPLIED"
    elif b == 7:
        if a == 5 and c == 2:
            addressMode = "ABSOLUTE_Y"
        else:
            addressMode = "ABSOLUTE_X"

    if c==1:
        if a==0:
            istruzione = "ORA();"
        elif a==1:
            istruzione = "AND();"
        elif a==2:
            istruzione = "EOR();"
        elif a==3:
            istruzione = "ADC();"
        elif a==4:
            istruzione = "STA();"
        elif a==5:
            istruzione = "LDA();"
        elif a==6:
            istruzione = "CMP();"
        else: 
            istruzione = "SBC();"
    elif c==2:
        if a==0:
            istruzione = "ASL();"
        elif a==1:
            istruzione = "ROL();"
        elif a==2:
            istruzione = "LSR();"
        elif a==3:
            istruzione = "ROR();"
        elif a==4:
            if b == 1:
                istruzione = "STX();"
            elif b==2:
                istruzione = "TXA();"
            elif b==3:
                istruzione = "STX();"
            elif b == 5:
                istruzione = "STX();"
            elif b==6:
                istruzione = "TXS();"
        elif a==5:
            if b==0:
                istruzione = "LDX();"
            elif((b%2)==1):
                istruzione = "LDX();"
            elif(b==2):
                istruzione = "TAX();"
            elif b==6:
                istruzione = "TSX();"
        elif a==6:
            istruzione = "DEC();"
        else: 
            if b==2:
                istruzione = "NOP();"
            else:
                istruzione = "INC();"
    elif c==0:
        istruzione = "//da scrivere"
    

    if (okNotOK(givenOpCode)==1):
        print(givenOpCode)
        riga1 = "        case {}:\n".format(hex(givenOpCode))
        #file1.write("     case",hex(givenOpCode),":")
        riga2 = "            //{}\n".format(addressMode)
        #file1.write("         //",addressMode)
        riga3 = "            {}\n".format(istruzione)
        ##file1.write("        ",istruzione)
        riga4 = "            break;\n"
        file1.write(riga1)
        file1.write(riga2)
        file1.write(riga3)
        file1.write(riga4)

file1.write("    }\n")
file1.write("}")