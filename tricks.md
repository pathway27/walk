On a gamefaqs walkthrough you can put this into the JavaScript console to get
a txt file.


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