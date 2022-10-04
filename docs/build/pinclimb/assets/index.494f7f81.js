(function(){const o=document.createElement("link").relList;if(o&&o.supports&&o.supports("modulepreload"))return;for(const i of document.querySelectorAll('link[rel="modulepreload"]'))t(i);new MutationObserver(i=>{for(const s of i)if(s.type==="childList")for(const a of s.addedNodes)a.tagName==="LINK"&&a.rel==="modulepreload"&&t(a)}).observe(document,{childList:!0,subtree:!0});function n(i){const s={};return i.integrity&&(s.integrity=i.integrity),i.referrerpolicy&&(s.referrerPolicy=i.referrerpolicy),i.crossorigin==="use-credentials"?s.credentials="include":i.crossorigin==="anonymous"?s.credentials="omit":s.credentials="same-origin",s}function t(i){if(i.ep)return;i.ep=!0;const s=n(i);fetch(i.href,s)}})();let c,l;function P(e,o){const n="#ddd",t=`
-webkit-touch-callout: none;
-webkit-tap-highlight-color: ${n};
-webkit-user-select: none;
-moz-user-select: none;
-ms-user-select: none;
user-select: none;
background: ${n};
color: #888;
`,i=`
position: absolute;
left: 50%;
top: 50%;
transform: translate(-50%, -50%);
`,s=`
image-rendering: -moz-crisp-edges;
image-rendering: -webkit-optimize-contrast;
image-rendering: -o-crisp-edges;
image-rendering: pixelated;
`;document.body.style.cssText=t,c=document.createElement("canvas"),c.width=e,c.height=o,l=c.getContext("2d"),l.imageSmoothingEnabled=!1,c.style.cssText=i+s,document.body.appendChild(c);const a=()=>{const S=innerWidth/innerHeight,p=c.width/c.height,k=S<p,F=k?.95*innerWidth:.95*innerHeight*p,O=k?.95*innerWidth/p:.95*innerHeight;c.style.width=`${F}px`,c.style.height=`${O}px`};window.addEventListener("resize",a),a()}function q(){l.fillStyle="#fff",l.fillRect(0,0,c.width,c.height),l.fillStyle="#000"}window.clearView=()=>{q()};function D(e,o,n,t){l.fillRect(Math.floor(e),Math.floor(o),Math.trunc(n),Math.trunc(t))}window.rect=(e,o,n,t)=>{D(e,o,n,t)};const r=6;let E,L;window.setTexts=(e,o)=>{let n=e;E=[];for(let t=0;t<o;t++)E.push(A(n)),n+=(r+1)*r};window.setCharacters=(e,o)=>{let n=e;L=[];for(let t=0;t<o;t++)L.push(A(n)),n+=(r+1)*r};window.text=(e,o,n)=>{const t=e-"!".charCodeAt(0);l.drawImage(E[t],Math.floor(o-r/2),Math.floor(n-r/2))};window.char=(e,o,n)=>{const t=e-"a".charCodeAt(0);l.drawImage(L[t],Math.floor(o-r/2),Math.floor(n-r/2))};let f,m;function H(){f=document.createElement("canvas"),f.width=f.height=r,m=f.getContext("2d")}function A(e){m.clearRect(0,0,r,r),m.fillStyle="#000";let o=e;for(let t=0;t<r;t++){for(let i=0;i<r;i++,o++)Module.HEAPU8[o]==="l".charCodeAt(0)&&m.fillRect(i,t,1,1);o++}const n=document.createElement("img");return n.src=f.toDataURL(),n}let w=!1,R=!1,T=!1,M=!1,x=!1,C=!1;function W(){document.addEventListener("mousedown",e=>{g()}),document.addEventListener("touchstart",e=>{g()}),document.addEventListener("keydown",e=>{g()}),document.addEventListener("mouseup",e=>{y()}),document.addEventListener("touchend",e=>{e.preventDefault(),e.target.click(),y()},{passive:!1}),document.addEventListener("keyup",e=>{y()})}function N(){R=!w&&x,T=w&&C,w=M,x=C=!1}function g(){M=x=!0}function y(){M=!1,C=!0}let d,h;function U(){d=new(window.AudioContext||window.webkitAudioContext),window.addEventListener("mousedown",v),window.addEventListener("touchstart",v),window.addEventListener("keydown",v),h=d.createGain(),h.gain.value=.05,h.connect(d.destination)}window.playTone=(e,o,n)=>{const t=d.createOscillator();t.type="square",t.frequency.value=e,t.start(n),t.stop(n+o),t.connect(h)};window.stopTone=()=>{};window.getAudioTime=()=>d.currentTime;function v(){d.resume()}function G(){N(),Module.ccall("setInput","void",["boolean","boolean","boolean"],[w,R,T]),Module.ccall("updateFrame","void",[],[])}const I=68,b=1e3/I;let u=0;function $(){requestAnimationFrame($);const e=window.performance.now();e<u-I/12||(u+=b,(u<e||u>e+b*2)&&(u=e+b),G())}function J(){H(),W(),U(),P(100,100),Module.ccall("initGame","void",[],[]),$()}window.Module={onRuntimeInitialized:J};const z=document.createElement("script");z.setAttribute("src","./wasm/cglp.js");document.head.appendChild(z);
