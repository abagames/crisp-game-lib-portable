(function(){const n=document.createElement("link").relList;if(n&&n.supports&&n.supports("modulepreload"))return;for(const t of document.querySelectorAll('link[rel="modulepreload"]'))i(t);new MutationObserver(t=>{for(const r of t)if(r.type==="childList")for(const s of r.addedNodes)s.tagName==="LINK"&&s.rel==="modulepreload"&&i(s)}).observe(document,{childList:!0,subtree:!0});function o(t){const r={};return t.integrity&&(r.integrity=t.integrity),t.referrerpolicy&&(r.referrerPolicy=t.referrerpolicy),t.crossorigin==="use-credentials"?r.credentials="include":t.crossorigin==="anonymous"?r.credentials="omit":r.credentials="same-origin",r}function i(t){if(t.ep)return;t.ep=!0;const r=o(t);fetch(t.href,r)}})();let c,l;function O(e,n){const o="#e0e0e0",i=`
-webkit-touch-callout: none;
-webkit-tap-highlight-color: ${o};
-webkit-user-select: none;
-moz-user-select: none;
-ms-user-select: none;
user-select: none;
background: ${o};
color: #888;
`,t=`
position: absolute;
left: 50%;
top: 50%;
transform: translate(-50%, -50%);
`,r=`
image-rendering: -moz-crisp-edges;
image-rendering: -webkit-optimize-contrast;
image-rendering: -o-crisp-edges;
image-rendering: pixelated;
`;document.body.style.cssText=i,c=document.createElement("canvas"),c.width=e,c.height=n,l=c.getContext("2d"),l.imageSmoothingEnabled=!1,c.style.cssText=t+r,document.body.appendChild(c);const s=()=>{const P=innerWidth/innerHeight,y=c.width/c.height,A=P<y,F=A?.95*innerWidth:.95*innerHeight*y,I=A?.95*innerWidth/y:.95*innerHeight;c.style.width=`${F}px`,c.style.height=`${I}px`};window.addEventListener("resize",s),s()}function U(e,n,o){l.fillStyle=p(e,n,o),l.fillRect(0,0,c.width,c.height)}window.clearView=(e,n,o)=>{U(e,n,o)};window.clearScreen=(e,n,o)=>{document.body.style.background=p(e,n,o)};function v(e){return e.toString(16).padStart(2,"0")}function p(e,n,o){return`#${v(e)}${v(n)}${v(o)}`}function q(e,n,o,i,t,r,s){l.fillStyle=p(t,r,s),l.fillRect(Math.floor(e),Math.floor(n),Math.trunc(o),Math.trunc(i))}window.drawRect=(e,n,o,i,t,r,s)=>{q(e,n,o,i,t,r,s)};const u=6;let C,f,m;function D(e){m.clearRect(0,0,u,u);let n=e;for(let i=0;i<u;i++)for(let t=0;t<u;t++,n+=3){const r=Module.HEAPU8[n],s=Module.HEAPU8[n+1],h=Module.HEAPU8[n+2];(r>0||s>0||h>0)&&(m.fillStyle=p(r,s,h),m.fillRect(t,i,1,1))}const o=document.createElement("img");return o.src=f.toDataURL(),o}window.drawCharacter=(e,n,o,i)=>{let t=C[i];t==null&&(t=D(e),C[i]=t),l.drawImage(t,Math.floor(n),Math.floor(o))};function W(){f=document.createElement("canvas"),f.width=f.height=u,m=f.getContext("2d"),C={}}let w=!1,R=!1,S=!1,$=!1,M=!1,k=!1;function N(){document.addEventListener("mousedown",e=>{b()}),document.addEventListener("touchstart",e=>{b()}),document.addEventListener("keydown",e=>{b()}),document.addEventListener("mouseup",e=>{E()}),document.addEventListener("touchend",e=>{e.preventDefault(),e.target.click(),E()},{passive:!1}),document.addEventListener("keyup",e=>{E()})}function G(){R=!w&&M,S=w&&k,w=$,M=k=!1}function b(){$=M=!0}function E(){$=!1,k=!0}let d,g;function J(){d=new(window.AudioContext||window.webkitAudioContext),window.addEventListener("mousedown",L),window.addEventListener("touchstart",L),window.addEventListener("keydown",L),g=d.createGain(),g.gain.value=.05,g.connect(d.destination)}window.playTone=(e,n,o)=>{const i=d.createOscillator();i.type="square",i.frequency.value=e,i.start(o),i.stop(o+n),i.connect(g)};window.stopTone=()=>{};window.getAudioTime=()=>d.currentTime;function L(){d.resume()}function V(){G(),Module.ccall("setInput","void",["boolean","boolean","boolean"],[w,R,S]),Module.ccall("updateFrame","void",[],[])}const T=68,x=1e3/T;let a=0;function z(){requestAnimationFrame(z);const e=window.performance.now();e<a-T/12||(a+=x,(a<e||a>e+x*2)&&(a=e+x),V())}function j(){W(),N(),J(),Module.ccall("initGame","void",[],[]),z()}window.initView=(e,n)=>{O(e,n)};window.Module={onRuntimeInitialized:j};const H=document.createElement("script");H.setAttribute("src","./wasm/cglp.js");document.head.appendChild(H);
