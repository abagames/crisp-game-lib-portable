(function(){const x=document.createElement("link").relList;if(x&&x.supports&&x.supports("modulepreload"))return;for(const f of document.querySelectorAll('link[rel="modulepreload"]'))P(f);new MutationObserver(f=>{for(const C of f)if(C.type==="childList")for(const D of C.addedNodes)D.tagName==="LINK"&&D.rel==="modulepreload"&&P(D)}).observe(document,{childList:!0,subtree:!0});function R(f){const C={};return f.integrity&&(C.integrity=f.integrity),f.referrerpolicy&&(C.referrerPolicy=f.referrerpolicy),f.crossorigin==="use-credentials"?C.credentials="include":f.crossorigin==="anonymous"?C.credentials="omit":C.credentials="same-origin",C}function P(f){if(f.ep)return;f.ep=!0;const C=R(f);fetch(f.href,C)}})();(function(){(function(n){function x(t,e=0,l=1){return Math.max(e,Math.min(t,l))}function R(t,e,l){const i=l-e,s=t-e;if(s>=0)return s%i+e;{let r=i+s%i+e;return r>=l&&(r-=i),r}}function P(t,e,l){return e<=t&&t<l}function f(t){return[...Array(t).keys()]}function C(t,e){return f(t).map(l=>e(l))}function D(t,e){let l=[];for(let i=0,s=0;i<t.length;s++)e(t[i],s)?(l.push(t[i]),t.splice(i,1)):i++;return l}function Kt(t){return[...t].reduce((e,[l,i])=>(e[l]=i,e),{})}function Nt(t){return Object.keys(t).map(e=>[e,t[e]])}function Tl(t,e){return String.fromCharCode(t.charCodeAt(0)+e)}function ue(t){return t.x!=null&&t.y!=null}class g{constructor(e,l){this.x=0,this.y=0,this.set(e,l)}set(e=0,l=0){return ue(e)?(this.x=e.x,this.y=e.y,this):(this.x=e,this.y=l,this)}add(e,l){return ue(e)?(this.x+=e.x,this.y+=e.y,this):(this.x+=e,this.y+=l,this)}sub(e,l){return ue(e)?(this.x-=e.x,this.y-=e.y,this):(this.x-=e,this.y-=l,this)}mul(e){return this.x*=e,this.y*=e,this}div(e){return this.x/=e,this.y/=e,this}clamp(e,l,i,s){return this.x=x(this.x,e,l),this.y=x(this.y,i,s),this}wrap(e,l,i,s){return this.x=R(this.x,e,l),this.y=R(this.y,i,s),this}addWithAngle(e,l){return this.x+=Math.cos(e)*l,this.y+=Math.sin(e)*l,this}swapXy(){const e=this.x;return this.x=this.y,this.y=e,this}normalize(){return this.div(this.length),this}rotate(e){if(e===0)return this;const l=this.x;return this.x=l*Math.cos(e)-this.y*Math.sin(e),this.y=l*Math.sin(e)+this.y*Math.cos(e),this}angleTo(e,l){return ue(e)?Math.atan2(e.y-this.y,e.x-this.x):Math.atan2(l-this.y,e-this.x)}distanceTo(e,l){let i,s;return ue(e)?(i=e.x-this.x,s=e.y-this.y):(i=e-this.x,s=l-this.y),Math.sqrt(i*i+s*s)}isInRect(e,l,i,s){return P(this.x,e,e+i)&&P(this.y,l,l+s)}equals(e){return this.x===e.x&&this.y===e.y}floor(){return this.x=Math.floor(this.x),this.y=Math.floor(this.y),this}round(){return this.x=Math.round(this.x),this.y=Math.round(this.y),this}ceil(){return this.x=Math.ceil(this.x),this.y=Math.ceil(this.y),this}get length(){return Math.sqrt(this.x*this.x+this.y*this.y)}get angle(){return Math.atan2(this.y,this.x)}}const _t=["transparent","white","red","green","yellow","blue","purple","cyan","black","light_red","light_green","light_yellow","light_blue","light_purple","light_cyan","light_black"],Al="twrgybpclRGYBPCL";let de;const Ul=[15658734,15277667,5025616,16761095,4149685,10233776,240116,6381921];function Jl(t){const[e,l,i]=it(0,t);if(de=Kt(_t.map((s,r)=>{if(r<1)return[s,{r:0,g:0,b:0,a:0}];if(r<9){const[c,h,m]=it(r-1,t);return[s,{r:c,g:h,b:m,a:1}]}const[o,a,u]=it(r-9+1,t);return[s,{r:Math.floor(t?o*.5:e-(e-o)*.5),g:Math.floor(t?a*.5:i-(i-a)*.5),b:Math.floor(t?u*.5:l-(l-u)*.5),a:1}]})),t){const s=de.blue;de.white={r:Math.floor(s.r*.15),g:Math.floor(s.g*.15),b:Math.floor(s.b*.15),a:1}}}function it(t,e){e&&(t===0?t=7:t===7&&(t=0));const l=Ul[t];return[(l&16711680)>>16,(l&65280)>>8,l&255]}function ie(t,e=1){const l=de[t];return Math.floor(l.r*e)<<16|Math.floor(l.g*e)<<8|Math.floor(l.b*e)}function fe(t,e=1){const l=de[t],i=Math.floor(l.r*e),s=Math.floor(l.g*e),r=Math.floor(l.b*e);return l.a<1?`rgba(${i},${s},${r},${l.a})`:`rgb(${i},${s},${r})`}const Kl=`
varying vec2 vTextureCoord;
uniform sampler2D uSampler;
uniform float width;
uniform float height;

float gridValue(vec2 uv, float res) {
  vec2 grid = fract(uv * res);
  return (step(res, grid.x) * step(res, grid.y));
}

void main(void) {
  vec4 color = texture2D(uSampler, vTextureCoord);  
  vec2 grid_uv = vTextureCoord.xy * vec2(width, height);
  float v = gridValue(grid_uv, 0.2);
  gl_FragColor.rgba = color * v;
}
`;function Nl(t,e){return new PIXI.Filter(void 0,Kl,{width:t,height:e})}const F=new g;let S,B,y,b=new g;const Ht=5;document.createElement("img");let w,he,ge=1,nt="black",k,Wt,ne=!1,p,qt;function _l(t,e,l,i,s,r,o){F.set(t),p=o,nt=l;const a=`
-webkit-touch-callout: none;
-webkit-tap-highlight-color: ${e};
-webkit-user-select: none;
-moz-user-select: none;
-ms-user-select: none;
user-select: none;
background: ${e};
color: #888;
`,u=`
position: absolute;
left: 50%;
top: 50%;
transform: translate(-50%, -50%);
`,c=`
image-rendering: -moz-crisp-edges;
image-rendering: -webkit-optimize-contrast;
image-rendering: -o-crisp-edges;
image-rendering: pixelated;
`;if(document.body.style.cssText=a,b.set(F),p.isUsingPixi){b.mul(Ht);const m=new PIXI.Application({width:b.x,height:b.y});if(S=m.view,y=new PIXI.Graphics,y.scale.x=y.scale.y=Ht,PIXI.settings.SCALE_MODE=PIXI.SCALE_MODES.NEAREST,m.stage.addChild(y),y.filters=[],p.name==="crt"&&y.filters.push(qt=new PIXI.filters.CRTFilter({vignettingAlpha:.7})),p.name==="pixel"&&y.filters.push(Nl(b.x,b.y)),p.name==="pixel"||p.name==="shapeDark"){const M=new PIXI.filters.AdvancedBloomFilter({threshold:.1,bloomScale:p.name==="pixel"?1.5:1,brightness:p.name==="pixel"?1.5:1,blur:8});y.filters.push(M)}y.lineStyle(0),S.style.cssText=u}else S=document.createElement("canvas"),S.width=b.x,S.height=b.y,B=S.getContext("2d"),B.imageSmoothingEnabled=!1,S.style.cssText=u+c;document.body.appendChild(S);const h=()=>{const M=innerWidth/innerHeight,E=b.x/b.y,q=M<E,I=q?.95*innerWidth:.95*innerHeight*E,et=q?.95*innerWidth/E:.95*innerHeight;S.style.width=`${I}px`,S.style.height=`${et}px`};if(window.addEventListener("resize",h),h(),i){w=document.createElement("canvas");let m;s?(w.width=b.x,w.height=b.y,m=r):(b.x<=b.y*2?(w.width=b.y*2,w.height=b.y):(w.width=b.x,w.height=b.x/2),w.width>400&&(ge=400/w.width,w.width=400,w.height*=ge),m=Math.round(400/w.width)),he=w.getContext("2d"),he.fillStyle=e,gcc.setOptions({scale:m,capturingFps:60,isSmoothingEnabled:!1})}}function Ie(){if(p.isUsingPixi){y.clear(),y.beginFill(ie(nt,p.isDarkColor?.15:1)),y.drawRect(0,0,F.x,F.y),y.endFill(),y.beginFill(ie(k)),ne=!0;return}B.fillStyle=fe(nt,p.isDarkColor?.15:1),B.fillRect(0,0,F.x,F.y),B.fillStyle=fe(k)}function T(t){if(t===k){p.isUsingPixi&&!ne&&De(ie(k));return}if(k=t,p.isUsingPixi){ne&&y.endFill(),De(ie(k));return}B.fillStyle=fe(t)}function De(t){Fe(),y.beginFill(t),ne=!0}function Fe(){ne&&(y.endFill(),ne=!1)}function $e(){Wt=k}function Le(){T(Wt)}function pe(t,e,l,i){if(p.isUsingPixi){p.name==="shape"||p.name==="shapeDark"?y.drawRoundedRect(t,e,l,i,2):y.drawRect(t,e,l,i);return}B.fillRect(t,e,l,i)}function Hl(t,e,l,i,s){const r=ie(k);De(r),y.drawCircle(t,e,s*.5),y.drawCircle(l,i,s*.5),Fe(),y.lineStyle(s,r),y.moveTo(t,e),y.lineTo(l,i),y.lineStyle(0)}function Wl(){qt.time+=.2}function ql(){if(he.fillRect(0,0,w.width,w.height),ge===1)he.drawImage(S,(w.width-S.width)/2,(w.height-S.height)/2);else{const t=S.width*ge,e=S.height*ge;he.drawImage(S,(w.width-t)/2,(w.height-e)/2,t,e)}gcc.capture(w)}const Xt=[`
l
l
l

l
`,`
l l
l l



`,`
 l l
lllll
 l l
lllll
 l l
`,`
 lll
l l
 lll
  l l
 lll
`,`
l   l
l  l
  l
 l  l
l   l
`,`
 l
l l
 ll l
l  l
 ll l
`,`
l
l



`,`
 l
l
l
l
 l
`,`
l
 l
 l
 l
l
`,`
  l
l l l
 lll
l l l
  l
`,`
  l
  l
lllll
  l
  l
`,`



 l
l
`,`


lllll


`,`




l
`,`
    l
   l
  l
 l
l
`,`
 lll
l  ll
l l l
ll  l
 lll
`,`
 ll
l l
  l
  l
lllll
`,`
 lll
l   l
  ll
 l
lllll
`,`
 lll
l   l
  ll
l   l
 lll
`,`
  ll
 l l
l  l
lllll
   l
`,`
lllll
l
llll
    l
llll
`,`
 lll
l
llll
l   l
 lll
`,`
lllll
l   l
   l
  l
 l
`,`
 lll
l   l
 lll
l   l
 lll
`,`
 lll
l   l
 llll
    l
 lll
`,`

l

l

`,`

 l

 l
l
`,`
   ll
 ll
l
 ll
   ll
`,`

lllll

lllll

`,`
ll
  ll
    l
  ll
ll
`,`
 lll
l   l
  ll

  l
`,`
 lll
l   l
l lll
l
 lll
`,`
 lll
l   l
lllll
l   l
l   l
`,`
llll
l   l
llll
l   l
llll
`,`
 lll
l   l
l
l   l
 lll
`,`
llll
l   l
l   l
l   l
llll
`,`
lllll
l
llll
l
lllll
`,`
lllll
l
llll
l
l
`,`
 lll
l
l  ll
l   l
 lll
`,`
l   l
l   l
lllll
l   l
l   l
`,`
lllll
  l
  l
  l
lllll
`,`
  lll
   l
   l
l  l
 ll
`,`
l   l
l  l
lll
l  l
l   l
`,`
l
l
l
l
lllll
`,`
l   l
ll ll
l l l
l   l
l   l
`,`
l   l
ll  l
l l l
l  ll
l   l
`,`
 lll
l   l
l   l
l   l
 lll
`,`
llll
l   l
llll
l
l
`,`
 lll
l   l
l   l
l  ll
 llll
`,`
llll
l   l
llll
l   l
l   l
`,`
 llll
l
 lll
    l
llll
`,`
lllll
  l
  l
  l
  l
`,`
l   l
l   l
l   l
l   l
 lll
`,`
l   l
l   l
l   l
 l l
  l
`,`
l   l
l l l
l l l
l l l
 l l
`,`
l   l
 l l
  l
 l l
l   l
`,`
l   l
 l l
  l
  l
  l
`,`
lllll
   l
  l
 l
lllll
`,`
  ll
  l
  l
  l
  ll
`,`
l
 l
  l
   l
    l
`,`
 ll
  l
  l
  l
 ll
`,`
  l
 l l



`,`




lllll
`,`
 l
  l



`,`

 lll
l  l
l  l
 lll
`,`
l
l
lll
l  l
lll
`,`

 lll
l  
l
 lll
`,`
   l
   l
 lll
l  l
 lll
`,`

 ll
l ll
ll
 ll
`,`
  l
 l 
lll
 l
 l
`,`
 ll
l  l
 lll
   l
 ll
`,`
l
l
ll
l l
l l
`,`

l

l
l
`,`
 l

 l
 l
l
`,`
l
l
l l
ll
l l
`,`
ll
 l
 l
 l
lll
`,`

llll
l l l
l l l
l   l
`,`

lll
l  l
l  l
l  l
`,`

 ll
l  l
l  l
 ll
`,`

lll
l  l
lll
l
`,`

 lll
l  l
 lll
   l
`,`

l ll
ll
l
l
`,`

 lll
ll
  ll
lll
`,`

 l
lll
 l
  l
`,`

l  l
l  l
l  l
 lll
`,`

l  l
l  l
 ll
 ll
`,`

l   l
l l l
l l l
 l l
`,`

l  l
 ll
 ll
l  l
`,`

l  l
 ll
 l
l
`,`

llll
  l
 l
llll
`,`
 ll
 l
l
 l
 ll
`,`
l
l
l
l
l
`,`
ll
 l
  l
 l
ll
`,`

 l
l l l
   l

`];let se,Be;function Xl(){se=[],Be=[]}function Vt(){se=se.concat(Be),Be=[]}function Yt(t){let e={isColliding:{rect:{},text:{},char:{}}};return se.forEach(l=>{Vl(t,l)&&(e=Object.assign(Object.assign(Object.assign({},e),st(l.collision.isColliding.rect)),{isColliding:{rect:Object.assign(Object.assign({},e.isColliding.rect),l.collision.isColliding.rect),text:Object.assign(Object.assign({},e.isColliding.text),l.collision.isColliding.text),char:Object.assign(Object.assign({},e.isColliding.char),l.collision.isColliding.char)}}))}),e}function Vl(t,e){const l=e.pos.x-t.pos.x,i=e.pos.y-t.pos.y;return-e.size.x<l&&l<t.size.x&&-e.size.y<i&&i<t.size.y}function st(t){if(t==null)return{};const e={transparent:"tr",white:"wh",red:"rd",green:"gr",yellow:"yl",blue:"bl",purple:"pr",cyan:"cy",black:"lc"};let l={};return Nt(t).forEach(([i,s])=>{const r=e[i];s&&r!=null&&(l[r]=!0)}),l}function Qt(t,e,l,i){return Zt(!1,t,e,l,i)}function Yl(t,e,l,i){return Zt(!0,t,e,l,i)}function Zt(t,e,l,i,s){if(typeof l=="number"){if(typeof i=="number")return ct(e,l,i,Object.assign({isCharacter:t,isCheckingCollision:!0,color:k},s));throw"invalid params"}else return ct(e,l.x,l.y,Object.assign({isCharacter:t,isCheckingCollision:!0,color:k},i))}const me=6,ye=1,d=me*ye;let el,rt,ot,at=!1,H,X,we,Te;const be={color:"black",backgroundColor:"transparent",rotation:0,mirror:{x:1,y:1},scale:{x:1,y:1},isCharacter:!1,isCheckingCollision:!1};function Ql(){H=document.createElement("canvas"),H.width=H.height=d,X=H.getContext("2d"),we=document.createElement("canvas"),Te=we.getContext("2d"),el=Xt.map((t,e)=>Object.assign(Object.assign({},dt(t)),{hitBox:Ae(String.fromCharCode(33+e),!1)})),rt=Xt.map((t,e)=>Object.assign(Object.assign({},dt(t)),{hitBox:Ae(String.fromCharCode(33+e),!0)})),ot={}}function Zl(t,e){const l=e.charCodeAt(0)-33;t.forEach((i,s)=>{rt[l+s]=Object.assign(Object.assign({},dt(i)),{hitBox:Ae(String.fromCharCode(33+l+s),!0)})})}function ei(){at=!0}function ct(t,e,l,i={}){const s=nl(i);e-=d/2*s.scale.x,l-=d/2*s.scale.y;const r=Math.floor(e);let o=t,a=r,u=Math.floor(l),c={isColliding:{rect:{},text:{},char:{}}};for(let h=0;h<o.length;h++){const m=o[h];if(m===`
`){a=r,u+=d*s.scale.y;continue}const M=tl(m,a,u,s);s.isCheckingCollision&&(c={isColliding:{rect:Object.assign(Object.assign({},c.isColliding.rect),M.isColliding.rect),text:Object.assign(Object.assign({},c.isColliding.text),M.isColliding.text),char:Object.assign(Object.assign({},c.isColliding.char),M.isColliding.char)}}),a+=d*s.scale.x}return c}function tl(t,e,l,i){const s=t.charCodeAt(0);if(s<32||s>126)return{isColliding:{rect:{},text:{},char:{}}};const r=nl(i);if(r.backgroundColor!=="transparent"&&($e(),T(r.backgroundColor),pe(e,l,d*r.scale.x,d*r.scale.y),Le()),s<=32)return{isColliding:{rect:{},text:{},char:{}}};const o=s-33,a=r.isCharacter?rt[o]:el[o],u=R(r.rotation,0,4);if(r.color==="black"&&u===0&&r.mirror.x===1&&r.mirror.y===1&&(!p.isUsingPixi||r.scale.x===1&&r.scale.y===1))return ut(a,e,l,r.scale,r.isCheckingCollision,!0);const c=JSON.stringify({c:t,options:r}),h=ot[c];if(h!=null)return ut(h,e,l,r.scale,r.isCheckingCollision,r.color!=="transparent");let m=!1;p.isUsingPixi&&(r.scale.x!==1||r.scale.y!==1)&&(we.width=d*r.scale.x,we.height=d*r.scale.y,Te.imageSmoothingEnabled=!1,Te.scale(r.scale.x,r.scale.y),ll(Te,u,r,a),m=!0),X.clearRect(0,0,d,d),ll(X,u,r,a);const M=Ae(t,r.isCharacter);let E;if(at||p.isUsingPixi){const q=document.createElement("img");if(q.src=H.toDataURL(),p.isUsingPixi){const I=document.createElement("img");I.src=(m?we:H).toDataURL(),E=PIXI.Texture.from(I)}at&&(ot[c]={image:q,texture:E,hitBox:M})}return ut({image:H,texture:E,hitBox:M},e,l,r.scale,r.isCheckingCollision,r.color!=="transparent")}function ll(t,e,l,i){e===0&&l.mirror.x===1&&l.mirror.y===1?t.drawImage(i.image,0,0):(t.save(),t.translate(d/2,d/2),t.rotate(Math.PI/2*e),(l.mirror.x===-1||l.mirror.y===-1)&&t.scale(l.mirror.x,l.mirror.y),t.drawImage(i.image,-d/2,-d/2),t.restore()),l.color!=="black"&&(t.globalCompositeOperation="source-in",t.fillStyle=fe(l.color==="transparent"?"black":l.color),t.fillRect(0,0,d,d),t.globalCompositeOperation="source-over")}function ut(t,e,l,i,s,r){if(r&&(i.x===1&&i.y===1?il(t,e,l):il(t,e,l,d*i.x,d*i.y)),!s)return;const o={pos:{x:e+t.hitBox.pos.x*i.x,y:l+t.hitBox.pos.y*i.y},size:{x:t.hitBox.size.x*i.x,y:t.hitBox.size.y*i.y},collision:t.hitBox.collision},a=Yt(o);return r&&se.push(o),a}function il(t,e,l,i,s){if(p.isUsingPixi){Fe(),y.beginTextureFill({texture:t.texture,matrix:new PIXI.Matrix().translate(e,l)}),y.drawRect(e,l,i==null?d:i,s==null?d:s),De(ie(k));return}i==null?B.drawImage(t.image,e,l):B.drawImage(t.image,e,l,i,s)}function dt(t,e=!0){X.clearRect(0,0,d,d);let l=t.split(`
`);e&&(l=l.slice(1,l.length-1));let i=0;l.forEach(u=>{i=Math.max(u.length,i)});const s=Math.max(Math.ceil((me-i)/2),0),r=l.length,o=Math.max(Math.ceil((me-r)/2),0);l.forEach((u,c)=>{if(!(c+o>=me))for(let h=0;h<me-s;h++){const m=u.charAt(h);let M=Al.indexOf(m);m!==""&&M>=1&&(X.fillStyle=fe(_t[M]),X.fillRect((h+s)*ye,(c+o)*ye,ye,ye))}});const a=document.createElement("img");return a.src=H.toDataURL(),p.isUsingPixi?{image:a,texture:PIXI.Texture.from(a)}:{image:a}}function Ae(t,e){const l={pos:new g(d,d),size:new g,collision:{isColliding:{char:{},text:{}}}};e?l.collision.isColliding.char[t]=!0:l.collision.isColliding.text[t]=!0;const i=X.getImageData(0,0,d,d).data;let s=0;for(let r=0;r<d;r++)for(let o=0;o<d;o++)i[s+3]>0&&(o<l.pos.x&&(l.pos.x=o),r<l.pos.y&&(l.pos.y=r)),s+=4;s=0;for(let r=0;r<d;r++)for(let o=0;o<d;o++)i[s+3]>0&&(o>l.pos.x+l.size.x-1&&(l.size.x=o-l.pos.x+1),r>l.pos.y+l.size.y-1&&(l.size.y=r-l.pos.y+1)),s+=4;return l}function nl(t){let e=Object.assign(Object.assign({},be),t);return t.scale!=null&&(e.scale=Object.assign(Object.assign({},be.scale),t.scale)),t.mirror!=null&&(e.mirror=Object.assign(Object.assign({},be.mirror),t.mirror)),e}let re=!1,Ue=!1,ft=!1;const sl=["Escape","Digit0","Digit1","Digit2","Digit3","Digit4","Digit5","Digit6","Digit7","Digit8","Digit9","Minus","Equal","Backspace","Tab","KeyQ","KeyW","KeyE","KeyR","KeyT","KeyY","KeyU","KeyI","KeyO","KeyP","BracketLeft","BracketRight","Enter","ControlLeft","KeyA","KeyS","KeyD","KeyF","KeyG","KeyH","KeyJ","KeyK","KeyL","Semicolon","Quote","Backquote","ShiftLeft","Backslash","KeyZ","KeyX","KeyC","KeyV","KeyB","KeyN","KeyM","Comma","Period","Slash","ShiftRight","NumpadMultiply","AltLeft","Space","CapsLock","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","Pause","ScrollLock","Numpad7","Numpad8","Numpad9","NumpadSubtract","Numpad4","Numpad5","Numpad6","NumpadAdd","Numpad1","Numpad2","Numpad3","Numpad0","NumpadDecimal","IntlBackslash","F11","F12","F13","F14","F15","F16","F17","F18","F19","F20","F21","F22","F23","F24","IntlYen","Undo","Paste","MediaTrackPrevious","Cut","Copy","MediaTrackNext","NumpadEnter","ControlRight","LaunchMail","AudioVolumeMute","MediaPlayPause","MediaStop","Eject","AudioVolumeDown","AudioVolumeUp","BrowserHome","NumpadDivide","PrintScreen","AltRight","Help","NumLock","Pause","Home","ArrowUp","PageUp","ArrowLeft","ArrowRight","End","ArrowDown","PageDown","Insert","Delete","OSLeft","OSRight","ContextMenu","BrowserSearch","BrowserFavorites","BrowserRefresh","BrowserStop","BrowserForward","BrowserBack"];let ht;const ti={onKeyDown:void 0};let gt,pt=!1,mt=!1,yt=!1,wt={},bt={},Ct={};function rl(t){gt=Object.assign(Object.assign({},ti),t),ht=Kt(sl.map(e=>[e,{isPressed:!1,isJustPressed:!1,isJustReleased:!1}])),document.addEventListener("keydown",e=>{pt=mt=!0,wt[e.code]=bt[e.code]=!0,gt.onKeyDown!=null&&gt.onKeyDown(),(e.code==="AltLeft"||e.code==="AltRight"||e.code==="ArrowRight"||e.code==="ArrowDown"||e.code==="ArrowLeft"||e.code==="ArrowUp")&&e.preventDefault()}),document.addEventListener("keyup",e=>{pt=!1,yt=!0,wt[e.code]=!1,Ct[e.code]=!0})}function ol(){Ue=!re&&mt,ft=re&&yt,mt=yt=!1,re=pt,Nt(ht).forEach(([t,e])=>{e.isJustPressed=!e.isPressed&&bt[t],e.isJustReleased=e.isPressed&&Ct[t],e.isPressed=!!wt[t]}),bt={},Ct={}}function al(){Ue=!1,re=!0}var li=Object.freeze({__proto__:null,get isPressed(){return re},get isJustPressed(){return Ue},get isJustReleased(){return ft},codes:sl,get code(){return ht},init:rl,update:ol,clearJustPressed:al});class Je{constructor(e=null){this.setSeed(e)}get(e=1,l){return l==null&&(l=e,e=0),this.next()/4294967295*(l-e)+e}getInt(e,l){l==null&&(l=e,e=0);const i=Math.floor(e),s=Math.floor(l);return s===i?i:this.next()%(s-i)+i}getPlusOrMinus(){return this.getInt(2)*2-1}select(e){return e[this.getInt(e.length)]}setSeed(e,l=123456789,i=362436069,s=521288629,r=32){this.w=e!=null?e>>>0:Math.floor(Math.random()*4294967295)>>>0,this.x=l>>>0,this.y=i>>>0,this.z=s>>>0;for(let o=0;o<r;o++)this.next();return this}getState(){return{x:this.x,y:this.y,z:this.z,w:this.w}}next(){const e=this.x^this.x<<11;return this.x=this.y,this.y=this.z,this.z=this.w,this.w=(this.w^this.w>>>19^(e^e>>>8))>>>0,this.w}}const Ce=new g;let A=!1,oe=!1,Se=!1,ii={isDebugMode:!1,anchor:new g,padding:new g,onPointerDownOrUp:void 0},O,z,v;const ve=new Je,V=new g,U=new g;let Me=!1,xe=new g,St=!1,vt=!1,Mt=!1;function cl(t,e,l){v=Object.assign(Object.assign({},ii),l),O=t,z=new g(e.x+v.padding.x*2,e.y+v.padding.y*2),xe.set(O.offsetLeft+O.clientWidth*(.5-v.anchor.x),O.offsetTop+O.clientWidth*(.5-v.anchor.y)),v.isDebugMode&&V.set(O.offsetLeft+O.clientWidth*(.5-v.anchor.x),O.offsetTop+O.clientWidth*(.5-v.anchor.y)),document.addEventListener("mousedown",i=>{fl(i.pageX,i.pageY)}),document.addEventListener("touchstart",i=>{fl(i.touches[0].pageX,i.touches[0].pageY)}),document.addEventListener("mousemove",i=>{hl(i.pageX,i.pageY)}),document.addEventListener("touchmove",i=>{i.preventDefault(),hl(i.touches[0].pageX,i.touches[0].pageY)},{passive:!1}),document.addEventListener("mouseup",i=>{gl()}),document.addEventListener("touchend",i=>{i.preventDefault(),i.target.click(),gl()},{passive:!1})}function ul(){ni(xe.x,xe.y,Ce),v.isDebugMode&&!Ce.isInRect(0,0,z.x,z.y)?(si(),Ce.set(V),oe=!A&&Me,Se=A&&!Me,A=Me):(oe=!A&&vt,Se=A&&Mt,A=St),vt=Mt=!1}function dl(){oe=!1,A=!0}function ni(t,e,l){O!=null&&(l.x=Math.round(((t-O.offsetLeft)/O.clientWidth+v.anchor.x)*z.x-v.padding.x),l.y=Math.round(((e-O.offsetTop)/O.clientHeight+v.anchor.y)*z.y-v.padding.y))}function si(){U.length>0?(V.add(U),!P(V.x,-z.x*.1,z.x*1.1)&&V.x*U.x>0&&(U.x*=-1),!P(V.y,-z.y*.1,z.y*1.1)&&V.y*U.y>0&&(U.y*=-1),ve.get()<.05&&U.set(0)):ve.get()<.1&&(U.set(0),U.addWithAngle(ve.get(Math.PI*2),(z.x+z.y)*ve.get(.01,.03))),ve.get()<.05&&(Me=!Me)}function fl(t,e){xe.set(t,e),St=vt=!0,v.onPointerDownOrUp!=null&&v.onPointerDownOrUp()}function hl(t,e){xe.set(t,e)}function gl(t){St=!1,Mt=!0,v.onPointerDownOrUp!=null&&v.onPointerDownOrUp()}var ri=Object.freeze({__proto__:null,pos:Ce,get isPressed(){return A},get isJustPressed(){return oe},get isJustReleased(){return Se},init:cl,update:ul,clearJustPressed:dl});let J=new g,K=!1,$=!1,W=!1;function pl(t){rl({onKeyDown:t}),cl(S,F,{onPointerDownOrUp:t,anchor:new g(.5,.5)})}function ml(){ol(),ul(),J=Ce,K=re||A,$=Ue||oe,W=ft||Se}function yl(){al(),dl()}function Pe(t){J.set(t.pos),K=t.isPressed,$=t.isJustPressed,W=t.isJustReleased}var oi=Object.freeze({__proto__:null,get pos(){return J},get isPressed(){return K},get isJustPressed(){return $},get isJustReleased(){return W},init:pl,update:ml,clearJustPressed:yl,set:Pe});let wl,bl;const Cl=68,xt=1e3/Cl;let ke=0;const ai={viewSize:{x:126,y:126},bodyBackground:"#111",viewBackground:"black",isUsingVirtualPad:!0,isFourWaysStick:!1,isCapturing:!1,isCapturingGameCanvasOnly:!1,isSoundEnabled:!0,captureCanvasScale:1,theme:{name:"simple",isUsingPixi:!1,isDarkColor:!1}};let j,Sl=10;function ci(t,e,l){wl=t,bl=e,j=Object.assign(Object.assign({},ai),l),Jl(j.theme.isDarkColor),_l(j.viewSize,j.bodyBackground,j.viewBackground,j.isCapturing,j.isCapturingGameCanvasOnly,j.captureCanvasScale,j.theme),pl(j.isSoundEnabled?sss.startAudio:()=>{}),Ql(),wl(),vl()}function vl(){requestAnimationFrame(vl);const t=window.performance.now();t<ke-Cl/12||(ke+=xt,(ke<t||ke>t+xt*2)&&(ke=t+xt),j.isSoundEnabled&&sss.update(),ml(),bl(),j.isCapturing&&ql(),Sl--,Sl===0&&ei())}class ui{constructor(e){this.size=new g,this.size.set(e),this.letterGrid=f(this.size.x).map(()=>f(this.size.y).map(()=>{})),this.colorGrid=f(this.size.x).map(()=>f(this.size.y).map(()=>{})),this.backgroundColorGrid=f(this.size.x).map(()=>f(this.size.y).map(()=>{})),this.rotationGrid=f(this.size.x).map(()=>f(this.size.y).map(()=>{})),this.characterGrid=f(this.size.x).map(()=>f(this.size.y).map(()=>{}))}print(e,l,i,s={}){const r=Object.assign(Object.assign({},be),s);let o=Math.floor(l),a=Math.floor(i);const u=o;for(let c=0;c<e.length;c++){const h=e[c];if(h===`
`){o=u,a++;continue}if(o<0||o>=this.size.x||a<0||a>=this.size.y){o++;continue}this.letterGrid[o][a]=h,this.colorGrid[o][a]=r.color,this.backgroundColorGrid[o][a]=r.backgroundColor,this.rotationGrid[o][a]=r.rotation,this.characterGrid[o][a]=r.isCharacter,o++}}getCharAt(e,l){if(e<0||e>=this.size.x||l<0||l>=this.size.y)return;const i=Math.floor(e),s=Math.floor(l),r=this.letterGrid[i][s],o=this.colorGrid[i][s],a=this.backgroundColorGrid[i][s],u=this.rotationGrid[i][s],c=this.characterGrid[i][s];return{char:r,options:{color:o,backgroundColor:a,rotation:u,isCharacter:c}}}setCharAt(e,l,i,s){if(e<0||e>=this.size.x||l<0||l>=this.size.y)return;const r=Object.assign(Object.assign({},be),s),o=Math.floor(e),a=Math.floor(l);this.letterGrid[o][a]=i,this.colorGrid[o][a]=r.color,this.backgroundColorGrid[o][a]=r.backgroundColor,this.rotationGrid[o][a]=r.rotation,this.characterGrid[o][a]=r.isCharacter}draw(){for(let e=0;e<this.size.x;e++)for(let l=0;l<this.size.y;l++){const i=this.letterGrid[e][l];if(i==null)continue;const s=this.colorGrid[e][l],r=this.backgroundColorGrid[e][l],o=this.rotationGrid[e][l],a=this.characterGrid[e][l];tl(i,e*d,l*d,{color:s,backgroundColor:r,rotation:o,isCharacter:a})}}clear(){for(let e=0;e<this.size.x;e++)for(let l=0;l<this.size.y;l++)this.letterGrid[e][l]=this.colorGrid[e][l]=this.backgroundColorGrid[e][l]=this.rotationGrid[e][l]=this.characterGrid[e][l]=void 0}scrollUp(){for(let l=0;l<this.size.x;l++)for(let i=1;i<this.size.y;i++)this.letterGrid[l][i-1]=this.letterGrid[l][i],this.colorGrid[l][i-1]=this.colorGrid[l][i],this.backgroundColorGrid[l][i-1]=this.backgroundColorGrid[l][i],this.rotationGrid[l][i-1]=this.rotationGrid[l][i],this.characterGrid[l][i-1]=this.characterGrid[l][i];const e=this.size.y-1;for(let l=0;l<this.size.x;l++)this.letterGrid[l][e]=this.colorGrid[l][e]=this.backgroundColorGrid[l][e]=this.rotationGrid[l][e]=this.characterGrid[l][e]=void 0}getState(){return{charGrid:this.letterGrid.map(e=>[].concat(e)),colorGrid:this.colorGrid.map(e=>[].concat(e)),backgroundColorGrid:this.backgroundColorGrid.map(e=>[].concat(e)),rotationGrid:this.rotationGrid.map(e=>[].concat(e)),symbolGrid:this.characterGrid.map(e=>[].concat(e))}}setState(e){this.letterGrid=e.charGrid.map(l=>[].concat(l)),this.colorGrid=e.colorGrid.map(l=>[].concat(l)),this.backgroundColorGrid=e.backgroundColorGrid.map(l=>[].concat(l)),this.rotationGrid=e.rotationGrid.map(l=>[].concat(l)),this.characterGrid=e.symbolGrid.map(l=>[].concat(l))}}let Ke;const Ne=new Je;function Pt(){Ke=[]}function Ml(t,e=16,l=1,i=0,s=Math.PI*2){if(e<1){if(Ne.get()>e)return;e=1}for(let r=0;r<e;r++){const o=i+Ne.get(s)-s/2,a={pos:new g(t),vel:new g(l*Ne.get(.5,1),0).rotate(o),color:k,ticks:x(Ne.get(10,20)*Math.sqrt(Math.abs(l)),10,60)};Ke.push(a)}}function _e(){$e(),Ke=Ke.filter(t=>(t.ticks--,t.ticks<0?!1:(t.pos.add(t.vel),t.vel.mul(.98),T(t.color),pe(Math.floor(t.pos.x),Math.floor(t.pos.y),1,1),!0))),Le()}function kt(t,e,l,i){return xl(!1,t,e,l,i)}function di(t,e,l,i){return xl(!0,t,e,l,i)}function fi(t,e,l,i,s=.5,r=.5){typeof t!="number"&&(r=s,s=i,i=l,l=e,e=t.y,t=t.x);const o=new g(l).rotate(s),a=new g(t-o.x*r,e-o.y*r);return Ot(a,o,i)}function hi(t,e,l=3,i=3,s=3){const r=new g,o=new g;if(typeof t=="number")if(typeof e=="number")typeof l=="number"?(r.set(t,e),o.set(l,i)):(r.set(t,e),o.set(l),s=i);else throw"invalid params";else if(typeof e=="number")if(typeof l=="number")r.set(t),o.set(e,l),s=i;else throw"invalid params";else if(typeof l=="number")r.set(t),o.set(e),s=l;else throw"invalid params";return Ot(r,o.sub(r),s)}function gi(t,e,l,i,s,r){let o=new g;typeof t=="number"?o.set(t,e):(o.set(t),r=s,s=i,i=l,l=e),i==null&&(i=3),s==null&&(s=0),r==null&&(r=Math.PI*2);let a,u;if(s>r?(a=r,u=s-r):(a=s,u=r-s),u=x(u,0,Math.PI*2),u<.01)return;const c=x(Math.ceil(u*Math.sqrt(l*.25)),1,36),h=u/c;let m=a,M=new g(l).rotate(m).add(o),E=new g,q=new g,I={isColliding:{rect:{},text:{},char:{}}};for(let et=0;et<c;et++){m+=h,E.set(l).rotate(m).add(o),q.set(E).sub(M);const tt=Ot(M,q,i,!0);I=Object.assign(Object.assign(Object.assign({},I),st(tt.isColliding.rect)),{isColliding:{rect:Object.assign(Object.assign({},I.isColliding.rect),tt.isColliding.rect),text:Object.assign(Object.assign({},I.isColliding.text),tt.isColliding.text),char:Object.assign(Object.assign({},I.isColliding.char),tt.isColliding.char)}}),M.set(E)}return Vt(),I}function xl(t,e,l,i,s){if(typeof e=="number"){if(typeof l=="number")return typeof i=="number"?s==null?Y(t,e,l,i,i):Y(t,e,l,i,s):Y(t,e,l,i.x,i.y);throw"invalid params"}else if(typeof l=="number"){if(i==null)return Y(t,e.x,e.y,l,l);if(typeof i=="number")return Y(t,e.x,e.y,l,i);throw"invalid params"}else return Y(t,e.x,e.y,l.x,l.y)}function Ot(t,e,l,i=!1){let s=!0;(p.name==="shape"||p.name==="shapeDark")&&(k!=="transparent"&&Hl(t.x,t.y,t.x+e.x,t.y+e.y,l),s=!1);const r=Math.floor(x(l,3,10)),o=Math.abs(e.x),a=Math.abs(e.y),u=x(Math.ceil(o>a?o/r:a/r)+1,3,99);e.div(u-1);let c={isColliding:{rect:{},text:{},char:{}}};for(let h=0;h<u;h++){const m=Y(!0,t.x,t.y,l,l,!0,s);c=Object.assign(Object.assign(Object.assign({},c),st(m.isColliding.rect)),{isColliding:{rect:Object.assign(Object.assign({},c.isColliding.rect),m.isColliding.rect),text:Object.assign(Object.assign({},c.isColliding.text),m.isColliding.text),char:Object.assign(Object.assign({},c.isColliding.char),m.isColliding.char)}}),t.add(e)}return i||Vt(),c}function Y(t,e,l,i,s,r=!1,o=!0){let a=o;(p.name==="shape"||p.name==="shapeDark")&&a&&k!=="transparent"&&(t?pe(e-i/2,l-s/2,i,s):pe(e,l,i,s),a=!1);let u=t?{x:Math.floor(e-i/2),y:Math.floor(l-s/2)}:{x:Math.floor(e),y:Math.floor(l)};const c={x:Math.trunc(i),y:Math.trunc(s)};if(c.x===0||c.y===0)return{isColliding:{rect:{},text:{},char:{}}};c.x<0&&(u.x+=c.x,c.x*=-1),c.y<0&&(u.y+=c.y,c.y*=-1);const h={pos:u,size:c,collision:{isColliding:{rect:{}}}};h.collision.isColliding.rect[k]=!0;const m=Yt(h);return k!=="transparent"&&((r?Be:se).push(h),a&&pe(u.x,u.y,c.x,c.y)),m}function Gt({pos:t,size:e,text:l,isToggle:i=!1,onClick:s=()=>{}}){return{pos:t,size:e,text:l,isToggle:i,onClick:s,isPressed:!1,isSelected:!1,isHovered:!1,toggleGroup:[]}}function Rt(t){const e=new g(J).sub(t.pos);t.isHovered=e.isInRect(0,0,t.size.x,t.size.y),t.isHovered&&oe&&(t.isPressed=!0),t.isPressed&&!t.isHovered&&(t.isPressed=!1),t.isPressed&&Se&&(t.onClick(),t.isPressed=!1,t.isToggle&&(t.toggleGroup.length===0?t.isSelected=!t.isSelected:(t.toggleGroup.forEach(l=>{l.isSelected=!1}),t.isSelected=!0))),He(t)}function He(t){$e(),T(t.isPressed?"blue":"light_blue"),kt(t.pos.x,t.pos.y,t.size.x,t.size.y),t.isToggle&&!t.isSelected&&(T("white"),kt(t.pos.x+1,t.pos.y+1,t.size.x-2,t.size.y-2)),T(t.isHovered?"black":"blue"),Qt(t.text,t.pos.x+3,t.pos.y+3),Le()}let L,Oe,Q,jt;function pi(t){L={randomSeed:t,inputs:[]}}function mi(t){L.inputs.push(t)}function Pl(){return L!=null}function yi(t){Oe=0,t.setSeed(L.randomSeed)}function wi(){Oe>=L.inputs.length||(Pe(L.inputs[Oe]),Oe++)}function bi(){Q=[]}function Ci(t,e,l){Q.push({randomState:l.getState(),gameState:cloneDeep(t),baseState:cloneDeep(e)})}function Si(t){const e=Q.pop(),l=e.randomState;return t.setSeed(l.w,l.x,l.y,l.z,0),jt={pos:new g(J),isPressed:K,isJustPressed:$,isJustReleased:W},Pe(L.inputs.pop()),e}function vi(t){const e=Q[Q.length-1],l=e.randomState;return t.setSeed(l.w,l.x,l.y,l.z,0),jt={pos:new g(J),isPressed:K,isJustPressed:$,isJustReleased:W},Pe(L.inputs[L.inputs.length-1]),e}function Mi(){Pe(jt)}function xi(){return Q.length===0}function Pi(){const t=Oe-1;if(!(t>=L.inputs.length))return Q[t]}const ki=Math.PI,Oi=Math.abs,Gi=Math.sin,Ri=Math.cos,ji=Math.atan2,zi=Math.sqrt,Ei=Math.pow,Ii=Math.floor,Di=Math.round,Fi=Math.ceil;n.ticks=0,n.difficulty=void 0,n.score=0,n.time=void 0,n.isReplaying=!1;function $i(t=1,e){return N.get(t,e)}function Li(t=2,e){return N.getInt(t,e)}function Bi(t=1,e){return N.get(t,e)*N.getPlusOrMinus()}function zt(t="GAME OVER"){Ze=t,ae&&(n.time=void 0),Il()}function Ti(t="COMPLETE"){Ze=t,Il()}function Ai(t,e,l){if(n.isReplaying||(n.score+=t,e==null))return;const i=`${t>=1?"+":""}${Math.floor(t)}`;let s=new g;typeof e=="number"?s.set(e,l):s.set(e),s.x-=i.length*d/2,s.y-=d/2,Ye.push({str:i,pos:s,vy:-2,ticks:30})}function kl(t){T(t)}function Ui(t,e,l,i,s,r){let o=new g;typeof t=="number"?(o.set(t,e),Ml(o,l,i,s,r)):(o.set(t),Ml(o,e,l,i,s))}function Ol(t,e){return new g(t,e)}function Gl(t,e){!ze&&!le&&te&&(e!=null&&typeof sss.playSoundEffect=="function"?sss.playSoundEffect(t,e):sss.play(Ni[t]))}let Et;function It(){typeof sss.generateMml=="function"?Et=sss.playMml(sss.generateMml()):sss.playBgm()}function Dt(){Et!=null&&sss.stopMml(Et),sss.stopBgm()}function Ji(t){if(ze){const e=vi(N),l=e.baseState;return n.score=l.score,n.ticks=l.ticks,cloneDeep(e.gameState)}else if(le){const e=Si(N),l=e.baseState;return n.score=l.score,n.ticks=l.ticks,e.gameState}else{if(n.isReplaying)return Pi().gameState;if(Z==="inGame"){const e={score:n.score,ticks:n.ticks};Ci(t,e,N)}}return t}function Ki(){le||(!n.isReplaying&&Ve?en():zt())}const Ni={coin:"c",laser:"l",explosion:"e",powerUp:"p",hit:"h",jump:"j",select:"s",lucky:"u",random:"r",click:"i",synth:"y",tone:"t"},Rl={isPlayingBgm:!1,isCapturing:!1,isCapturingGameCanvasOnly:!1,captureCanvasScale:1,isShowingScore:!0,isShowingTime:!1,isReplayEnabled:!1,isRewindEnabled:!1,isDrawingParticleFront:!1,isDrawingScoreFront:!1,isMinifying:!1,isSoundEnabled:!0,viewSize:{x:100,y:100},seed:0,theme:"simple"},_i=new Je,N=new Je;let Z,Hi={title:Yi,inGame:Vi,gameOver:Qi,rewind:tn},G,Ft=0,We,qe=!0,Xe=0,ee,Ge,jl,ae,Re,Ve,je,$t,te,_,Ye,ze=!1,le=!1,Ee,Qe,Ze,Lt;function Wi(t){const e=window;e.update=t.update,e.title=t.title,e.description=t.description,e.characters=t.characters,e.options=t.options,zl()}function zl(){let t;typeof options<"u"&&options!=null?t=Object.assign(Object.assign({},Rl),options):t=Rl;const e={name:t.theme,isUsingPixi:!1,isDarkColor:!1};t.theme!=="simple"&&t.theme!=="dark"&&(e.isUsingPixi=!0),(t.theme==="pixel"||t.theme==="shapeDark"||t.theme==="crt"||t.theme==="dark")&&(e.isDarkColor=!0),ee={viewSize:{x:100,y:100},bodyBackground:e.isDarkColor?"#101010":"#e0e0e0",viewBackground:e.isDarkColor?"blue":"white",theme:e,isSoundEnabled:t.isSoundEnabled},Xe=t.seed,ee.isCapturing=t.isCapturing,ee.isCapturingGameCanvasOnly=t.isCapturingGameCanvasOnly,ee.captureCanvasScale=t.captureCanvasScale,ee.viewSize=t.viewSize,Ge=t.isPlayingBgm,jl=t.isShowingScore&&!t.isShowingTime,ae=t.isShowingTime,Re=t.isReplayEnabled,Ve=t.isRewindEnabled,je=t.isDrawingParticleFront,$t=t.isDrawingScoreFront,te=t.isSoundEnabled,t.isMinifying&&sn(),ci(qi,Xi,ee)}function qi(){typeof description<"u"&&description!=null&&description.trim().length>0&&(qe=!1,Xe+=$l(description)),typeof title<"u"&&title!=null&&title.trim().length>0&&(qe=!1,document.title=title,Xe+=$l(title)),typeof characters<"u"&&characters!=null&&Zl(characters,"a"),te&&sss.init(Xe);const t=ee.viewSize;_={x:Math.floor(t.x/6),y:Math.floor(t.y/6)},G=new ui(_),T("black"),qe?(Bt(),n.ticks=0):El()}function Xi(){n.df=n.difficulty=n.ticks/3600+1,n.tc=n.ticks;const t=n.score,e=n.time;n.sc=n.score;const l=n.sc;n.inp={p:J,ip:K,ijp:$,ijr:W},Xl(),Hi[Z](),p.isUsingPixi&&(Fe(),p.name==="crt"&&Wl()),n.ticks++,n.isReplaying?(n.score=t,n.time=e):n.sc!==l&&(n.score=n.sc)}function Bt(){Z="inGame",n.ticks=-1,Pt();const t=Math.floor(n.score);t>Ft&&(Ft=t),ae&&n.time!=null&&(We==null||We>n.time)&&(We=n.time),n.score=0,n.time=0,Ye=[],Ge&&te&&It();const e=_i.getInt(999999999);N.setSeed(e),(Re||Ve)&&(pi(e),bi(),n.isReplaying=!1)}function Vi(){G.clear(),Ie(),je||_e(),$t||Fl(),(Re||Ve)&&mi({pos:Ol(J),isPressed:K,isJustPressed:$,isJustReleased:W}),typeof update=="function"&&update(),je&&_e(),$t&&Fl(),Tt(),G.draw(),ae&&n.time!=null&&n.time++}function El(){Z="title",n.ticks=-1,Pt(),G.clear(),Ie(),Pl()&&(yi(N),n.isReplaying=!0)}function Yi(){if($){Bt();return}if(Ie(),Re&&Pl()&&(wi(),n.inp={p:J,ip:K,ijp:$,ijr:W},je||_e(),update(),je&&_e()),n.ticks===0&&(Tt(),typeof title<"u"&&title!=null&&G.print(title,Math.floor(_.x-title.length)/2,Math.ceil(_.y*.2))),(n.ticks===30||n.ticks==40)&&typeof description<"u"&&description!=null){let t=0;description.split(`
`).forEach(l=>{l.length>t&&(t=l.length)});const e=Math.floor((_.x-t)/2);description.split(`
`).forEach((l,i)=>{G.print(l,e,Math.floor(_.y/2)+i)})}G.draw()}function Il(){Z="gameOver",n.isReplaying||yl(),n.ticks=-1,Zi(),Ge&&te&&Dt()}function Qi(){(n.isReplaying||n.ticks>20)&&$?Bt():n.ticks===(Re?120:300)&&!qe&&El()}function Zi(){n.isReplaying||(G.print(Ze,Math.floor((_.x-Ze.length)/2),Math.floor(_.y/2)),G.draw())}function en(){Z="rewind",ze=!0,Ee=Gt({pos:{x:F.x-39,y:11},size:{x:36,y:7},text:"Rewind"}),Qe=Gt({pos:{x:F.x-39,y:F.y-19},size:{x:36,y:7},text:"GiveUp"}),Ge&&te&&Dt(),p.isUsingPixi&&(He(Ee),He(Qe))}function tn(){G.clear(),Ie(),update(),Tt(),Mi(),le?(He(Ee),(xi()||!K)&&ln()):(Rt(Ee),Rt(Qe),Ee.isPressed&&(le=!0,ze=!1)),Qe.isPressed?(ze=le=!1,zt()):G.draw(),ae&&n.time!=null&&n.time++}function ln(){le=!1,Z="inGame",Pt(),Ge&&te&&It()}function Tt(){if(jl){G.print(`${Math.floor(n.score)}`,0,0);const t=`HI ${Ft}`;G.print(t,_.x-t.length,0)}ae&&(Dl(n.time,0,0),Dl(We,9,0))}function Dl(t,e,l){if(t==null)return;let i=Math.floor(t*100/50);i>=10*60*100&&(i=10*60*100-1);const s=At(Math.floor(i/6e3),1)+"'"+At(Math.floor(i%6e3/100),2)+'"'+At(Math.floor(i%100),2);G.print(s,e,l)}function At(t,e){return("0000"+t).slice(-e)}function Fl(){$e(),T("black"),Ye=Ye.filter(t=>(ct(t.str,t.pos.x,t.pos.y),t.pos.y+=t.vy,t.vy*=.9,t.ticks--,t.ticks>0)),Le()}function $l(t){let e=0;for(let l=0;l<t.length;l++){const i=t.charCodeAt(l);e=(e<<5)-e+i,e|=0}return e}function nn(){let t=window.location.search.substring(1);if(t=t.replace(/[^A-Za-z0-9_-]/g,""),t.length===0)return;const e=document.createElement("script");Lt=`${t}/main.js`,e.setAttribute("src",Lt),document.head.appendChild(e)}function sn(){fetch(Lt).then(t=>t.text()).then(t=>{const e=Terser.minify(t+"update();",{toplevel:!0}).code,l="function(){",i=e.indexOf(l),s="options={",r=e.indexOf(s);let o=e;if(i>=0)o=e.substring(e.indexOf(l)+l.length,e.length-4);else if(r>=0){let a=1,u;for(let c=r+s.length;c<e.length;c++){const h=e.charAt(c);if(h==="{")a++;else if(h==="}"&&(a--,a===0)){u=c+2;break}}a===0&&(o=e.substring(u))}Ll.forEach(([a,u])=>{o=o.split(a).join(u)}),console.log(o),console.log(`${o.length} letters`)})}n.inp=void 0;function rn(...t){return kl.apply(this,t)}function on(...t){return Gl.apply(this,t)}function an(...t){return C.apply(this,t)}function cn(...t){return D.apply(this.args)}n.tc=void 0,n.df=void 0,n.sc=void 0;const un="transparent",dn="white",fn="red",hn="green",gn="yellow",pn="blue",mn="purple",yn="cyan",wn="black",bn="coin",Cn="laser",Sn="explosion",vn="powerUp",Mn="hit",xn="jump",Pn="select",kn="lucky";let Ll=[["===","=="],["!==","!="],["input.pos","inp.p"],["input.isPressed","inp.ip"],["input.isJustPressed","inp.ijp"],["input.isJustReleased","inp.ijr"],["color(","clr("],["play(","ply("],["times(","tms("],["remove(","rmv("],["ticks","tc"],["difficulty","df"],["score","sc"],[".isColliding.rect.transparent",".tr"],[".isColliding.rect.white",".wh"],[".isColliding.rect.red",".rd"],[".isColliding.rect.green",".gr"],[".isColliding.rect.yellow",".yl"],[".isColliding.rect.blue",".bl"],[".isColliding.rect.purple",".pr"],[".isColliding.rect.cyan",".cy"],[".isColliding.rect.black",".lc"],['"transparent"',"tr"],['"white"',"wh"],['"red"',"rd"],['"green"',"gr"],['"yellow"',"yl"],['"blue"',"bl"],['"purple"',"pr"],['"cyan"',"cy"],['"black"',"lc"],['"coin"',"cn"],['"laser"',"ls"],['"explosion"',"ex"],['"powerUp"',"pw"],['"hit"',"ht"],['"jump"',"jm"],['"select"',"sl"],['"lucky"',"uc"]];n.PI=ki,n.abs=Oi,n.addGameScript=nn,n.addScore=Ai,n.addWithCharCode=Tl,n.arc=gi,n.atan2=ji,n.bar=fi,n.bl=pn,n.box=di,n.ceil=Fi,n.char=Yl,n.clamp=x,n.clr=rn,n.cn=bn,n.color=kl,n.complete=Ti,n.cos=Ri,n.cy=yn,n.end=zt,n.ex=Sn,n.floor=Ii,n.frameState=Ji,n.getButton=Gt,n.gr=hn,n.ht=Mn,n.init=Wi,n.input=oi,n.jm=xn,n.keyboard=li,n.lc=wn,n.line=hi,n.ls=Cn,n.minifyReplaces=Ll,n.onLoad=zl,n.particle=Ui,n.play=Gl,n.playBgm=It,n.ply=on,n.pointer=ri,n.pow=Ei,n.pr=mn,n.pw=vn,n.range=f,n.rd=fn,n.rect=kt,n.remove=D,n.rewind=Ki,n.rmv=cn,n.rnd=$i,n.rndi=Li,n.rnds=Bi,n.round=Di,n.sin=Gi,n.sl=Pn,n.sqrt=zi,n.stopBgm=Dt,n.text=Qt,n.times=C,n.tms=an,n.tr=un,n.uc=kn,n.updateButton=Rt,n.vec=Ol,n.wh=dn,n.wrap=R,n.yl=gn,Object.defineProperty(n,"__esModule",{value:!0})})(window||{})})();function On(){Module.ccall("setInput","void",["boolean","boolean","boolean"],[input.isPressed,input.isJustPressed,input.isJustReleased]),Module.ccall("updateFrame","void",[],[])}let Jt=[];function Gn(){Rn(),Module.ccall("initGame","void",[],[]),init({update:On,characters:Jt,options:{isSoundEnabled:!1,isShowingScore:!1}})}window.setCharacters=(n,x)=>{let R=n;Jt=[];for(let P=0;P<x;P++){let f=`
`;for(let C=0;C<6;C++){for(let D=0;D<6;D++)f+=String.fromCharCode(Module.HEAPU8[R+D]);R+=7,f+=`
`}Jt.push(f)}};let ce,lt;function Rn(){ce=new(window.AudioContext||window.webkitAudioContext),window.addEventListener("mousedown",Ut),window.addEventListener("touchstart",Ut),window.addEventListener("keydown",Ut),lt=ce.createGain(),lt.gain.value=.05,lt.connect(ce.destination)}window.playTone=(n,x,R)=>{const P=ce.createOscillator();P.type="square",P.frequency.value=n,P.start(R),P.stop(R+x),P.connect(lt)};window.stopTone=()=>{};window.getAudioTime=()=>ce.currentTime;function Ut(){ce.resume()}window.Module={onRuntimeInitialized:Gn};const Bl=document.createElement("script");Bl.setAttribute("src","./wasm/cglp.js");document.head.appendChild(Bl);
