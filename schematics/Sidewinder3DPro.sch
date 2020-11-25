EESchema Schematic File Version 4
LIBS:Sidewinder3DPro-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Microsoft Sidewinder 3D Pro USB Adapter"
Date "2020-11-25"
Rev "1.0"
Comp "www.franken.tech"
Comment1 "Cristiano José Sulzbach"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L teensy:Teensy++2.0_(C) U1
U 1 1 5FBD64F9
P 7350 3900
F 0 "U1" H 7350 5537 60  0000 C CNN
F 1 "Teensy++2.0_(C)" H 7350 5431 60  0000 C CNN
F 2 "" H 7450 3050 60  0000 C CNN
F 3 "" H 7450 3050 60  0000 C CNN
	1    7350 3900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5FBD7BCD
P 4200 3150
F 0 "R1" V 3993 3150 50  0000 C CNN
F 1 "2k2" V 4084 3150 50  0000 C CNN
F 2 "" V 4130 3150 50  0001 C CNN
F 3 "~" H 4200 3150 50  0001 C CNN
	1    4200 3150
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 5FBD92C7
P 4200 3650
F 0 "R2" V 3993 3650 50  0000 C CNN
F 1 "2k2" V 4084 3650 50  0000 C CNN
F 2 "" V 4130 3650 50  0001 C CNN
F 3 "~" H 4200 3650 50  0001 C CNN
	1    4200 3650
	0    1    1    0   
$EndComp
$Comp
L Connector:DB15_Female J1
U 1 1 5FBDE4DF
P 3100 3350
F 0 "J1" H 3006 2358 50  0000 C CNN
F 1 "DB15_Female" H 3006 2449 50  0000 C CNN
F 2 "" H 3100 3350 50  0001 C CNN
F 3 " ~" H 3100 3350 50  0001 C CNN
	1    3100 3350
	-1   0    0    1   
$EndComp
Wire Wire Line
	3400 3150 4050 3150
Wire Wire Line
	3400 3650 4050 3650
Wire Wire Line
	6300 4550 3400 4550
Wire Wire Line
	3400 4550 3400 4050
Wire Wire Line
	3400 3450 5550 3450
Wire Wire Line
	5550 3450 5550 2550
Wire Wire Line
	5550 2550 6300 2550
Wire Wire Line
	4350 3650 5550 3650
Wire Wire Line
	5550 3650 5550 5650
Wire Wire Line
	5550 5650 8700 5650
Wire Wire Line
	8700 5650 8700 4050
Wire Wire Line
	8700 4050 8400 4050
Wire Wire Line
	4350 3150 5750 3150
Wire Wire Line
	5750 3150 5750 5500
Wire Wire Line
	5750 5500 8600 5500
Wire Wire Line
	8600 5500 8600 4150
Wire Wire Line
	8600 4150 8400 4150
Wire Wire Line
	3400 3850 5950 3850
Wire Wire Line
	5950 3850 5950 2750
Wire Wire Line
	5950 2750 6300 2750
Wire Wire Line
	3400 2850 6300 2850
Wire Wire Line
	3400 3750 6100 3750
Wire Wire Line
	6100 3750 6100 2950
Wire Wire Line
	6100 2950 6300 2950
Wire Wire Line
	3400 2950 5850 2950
Wire Wire Line
	5850 2950 5850 3050
Wire Wire Line
	5850 3050 6300 3050
$EndSCHEMATC
