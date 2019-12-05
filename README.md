# Sample PineTime project using Zephyr OS

Currently it shows seconds elapsed since

## Prerequisites

* [Zephyr RTOS](https://docs.zephyrproject.org/latest/)
* [PineTime Board Definition](https://github.com/zephyrproject-rtos/zephyr/pull/19973) (+ see below)
* [Updated ST7789V driver](https://github.com/zephyrproject-rtos/zephyr/pull/20570)


## PineTime definition
Check your pinetime .dts, project assumes following entries: (should make it an overlay or something)

```
/ {

	...

	aliases {
		led0 = &led0;
		blled0 = &blled1;
		blled1 = &blled1;
		blled2 = &blled1;
		status-led= &led0;
		sw0 = &button0;
		key-in = &button0;
	};

	leds {
		compatible = "gpio-leds";
		led0: led_0 {
			gpios = <&gpio0 27 0>;
			label = "Green LED";
		};
		blled0: bl_led_0 {
			gpios = <&gpio0 14 GPIO_INT_ACTIVE_LOW>;
			label = "Backlight LED 0";
		};
		blled1: bl_led_1 {
			gpios = <&gpio0 22 GPIO_INT_ACTIVE_LOW>;
			label = "Backlight LED 1";
		};
		blled2: bl_led_2 {
			gpios = <&gpio0 23 GPIO_INT_ACTIVE_LOW>;
			label = "Backlight LED 2";
		};
	};

	...

};

...

&spi1 {
	compatible = "nordic,nrf-spi";
	status = "okay";
	sck-pin = <2>;
	mosi-pin = <3>;
	miso-pin = <4>;

	cs-gpios = <&gpio0 5 0>, <&gpio0 25 0>;

	...

	/* Sitronix ST7789V LCD */
	st7789v: st7789v@1 {
		compatible = "sitronix,st7789v";
		reg = <1>;
		spi-max-frequency = <8000000>; /* 8MHz */
		label = "ST7789V";
		cmd-data-gpios = <&gpio0 18 0>;	/* DET */
		reset-gpios = <&gpio0 26 0>;	/* RESX reset */
		width = <240>;
		height = <240>;
		x-offset = <0>;
		y-offset = <0>;
		vcom = <0x19>;
		gctrl = <0x35>;
		vrhs = <0x12>;
		vdvs = <0x20>;
		mdac = <0x00>;
		gamma = <0x01>;
		colmod = <0x05>;
		lcm = <0x2c>;
		porch-param = [0c 0c 00 33 33];
		cmd2en-param = [5a 69 02 01];
		pwctrl1-param = [a4 a1];
		pvgam-param = [D0 04 0D 11 13 2B 3F 54 4C 18 0D 0B 1F 23];
		nvgam-param = [D0 04 0C 11 13 2C 3F 44 51 2F 1F 1F 20 23];
		ram-param = [00 F0];
		rgb-param = [CD 08 14];
	};
};
```

**NOTE**: I have no idea what's going on in those parameters, they may or may not be incorrect.

## See Also
[Repository with instructions and examples and different patches](https://github.com/najnesnaj/pinetime-zephyr)
