## Walk

Is a PSVITA plugin to read a walkthrough while playing a game.

## Walk plugin

### Installation

This plugin needs [IO Plus](https://github.com/CelesteBlue-dev/PSVita-RE-tools/raw/master/ioPlus/ioPlus-0.1/release/ioplus.skprx) by CelesteBlue.

Then download this plugin (walk.suprx) and add to your `ux0:tai/config.txt`.
Add it to `*ALL` to enable it for all games, or add a section for the titleid
of the game you're interested in.

```
*KERNAL
ux0:/tai/ioplus.skprx
...
*ALL
ux0:tai/walk.suprx
```

Add your `txt` file into the directory (with filename as the game's TITLEID): `ux0:data/walks/TITLEID.txt`.

You can find the title id at the gamefaqs' data page, like [this one](https://gamefaqs.gamespot.com/vita/763296-danganronpa-2-goodbye-despair/data).

### Usage

Open a Game and then Hold `Select + R Trigger` to open/close the reader.

Use the D-Pad to control the viewer:
  - Up    - Go up a line
  - Down  - Go down a line
  - Left  - Go up 27 lines
  - Right - Go down 27 lines

NOTE: The viewer doesn't pause the game, it's still running behind!


### TODO:

- Bigger font / custom font
- Use Network for guides / server
- More than one file per titleid
- detect from emulators / psp mode
- html

## Tricks

### Getting a FAQ from gamefaqs.com

Find a gamefaqs walkthrough you want like [this one](https://gamefaqs.gamespot.com/vita/763296-danganronpa-2-goodbye-despair/faqs/70277).

[Open the Javascript Console](https://webmasters.stackexchange.com/questions/8525/how-do-i-open-the-javascript-console-in-different-browsers).

Use this JavaScript code to get a `txt` file, which you can rename and put onto your
vita.

```js
(function() {
let file = new Blob([$(".faqtext").text()], {type: "application/text"});
let a = document.createElement("a"),
url = URL.createObjectURL(file);
a.href = url;
a.download = window.document.title + ".txt";
document.body.appendChild(a);
a.click();
})()
```

## License

GPLv3

## Disclaimer

By using this app / plugin you agree that you are reading the guides provided
by the stated user as if on on the website `gamefaqs.com`.

## Thanks

- CelesteBlue for [ioplus](https://github.com/CelesteBlue-dev/PSVita-RE-tools/tree/master/ioPlus/ioPlus-0.1)
- Electry for [VGi](https://github.com/Electry/VGi)
- Molecule Team for [Henkaku](https://henkaku.xyz/) and yifanlu [TaiHen](https://tai.henkaku.xyz/)
- Daniel Hepper for [font8x8](https://github.com/dhepper/font8x8)
- vitasdk peeps