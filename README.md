## Walk

Is a PSVITA plugin to read a `txt` file walkthrough while playing a game.
> Too many games, not enough time

**This is part of the [KyûHEN competition](https://kyuhen.customprotocol.com/en/).**

![](https://i.imgur.com/7Kb7sgQ.png)

```
vvalk
i
t
a
```

> Insert pun for pathway and walk. Walk on the pathway.

See `plugin/README.md` for development help.

### Installation

This plugin needs [IO Plus](https://github.com/CelesteBlue-dev/PSVita-RE-tools/raw/master/ioPlus/ioPlus-0.1/release/ioplus.skprx) by CelesteBlue.

Then download `walk` and put the `.suprx` file in `ux0:tai` and it to
your `ux0:tai/config.txt`.

Add it to `*ALL` to enable it for all games, or add a section for the titleid
of the game / app you're interested in.

```
*KERNAL
ux0:/tai/ioplus.skprx
...
*ALL
ux0:tai/walk.suprx
```

Transfer your `txt` into the directory `ux0:data/walks/TITLEID/` with the filename as `walk.txt`.

So the full path will be: `ux0:data/walks/TITLEID/walk.txt`

You can find the title id at the gamefaqs' data page, like [this one](https://gamefaqs.gamespot.com/vita/763296-danganronpa-2-goodbye-despair/data).

### Usage

Open a Game and then Hold `Select + R Trigger` to open/close the reader.

Use the D-Pad to control the viewer:
  - Up    - Go up a line
  - Down  - Go down a line
  - Left  - Go up 27 lines
  - Right - Go down 27 lines

NOTE: The viewer doesn't pause the game, it's still running behind it!
NOTE: Some lines maybe cut-off and you may need to use Left and Right to go to the next page

### TODO:

- Some characters are broken like `¯`, `U+00AF`
- Bigger font / custom font
- More than one walkthrough / file per titleid
- Use Network for guides / guides API server
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

## Disclaimer

By doing above and using this plugin you agree that you are reading guides provided
by the author as if on on the website `gamefaqs.com`.

## License

GPLv3

## Thanks

- Daniel Hepper for [font8x8](https://github.com/dhepper/font8x8)
- CelesteBlue for [ioplus](https://github.com/CelesteBlue-dev/PSVita-RE-tools/tree/master/ioPlus/ioPlus-0.1)
- Electry for [VGi](https://github.com/Electry/VGi)
- Molecule Team for [Henkaku](https://henkaku.xyz/) and yifanlu for [TaiHen](https://tai.henkaku.xyz/)
- vitasdk peeps