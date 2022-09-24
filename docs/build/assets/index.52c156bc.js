(function(){const P=document.createElement("link").relList;if(P&&P.supports&&P.supports("modulepreload"))return;for(const f of document.querySelectorAll('link[rel="modulepreload"]'))E(f);new MutationObserver(f=>{for(const C of f)if(C.type==="childList")for(const I of C.addedNodes)I.tagName==="LINK"&&I.rel==="modulepreload"&&E(I)}).observe(document,{childList:!0,subtree:!0});function B(f){const C={};return f.integrity&&(C.integrity=f.integrity),f.referrerpolicy&&(C.referrerPolicy=f.referrerpolicy),f.crossorigin==="use-credentials"?C.credentials="include":f.crossorigin==="anonymous"?C.credentials="omit":C.credentials="same-origin",C}function E(f){if(f.ep)return;f.ep=!0;const C=B(f);fetch(f.href,C)}})();(function(){(function(n){function P(t,e=0,l=1){return Math.max(e,Math.min(t,l))}function B(t,e,l){const i=l-e,s=t-e;if(s>=0)return s%i+e;{let r=i+s%i+e;return r>=l&&(r-=i),r}}function E(t,e,l){return e<=t&&t<l}function f(t){return[...Array(t).keys()]}function C(t,e){return f(t).map(l=>e(l))}function I(t,e){let l=[];for(let i=0,s=0;i<t.length;s++)e(t[i],s)?(l.push(t[i]),t.splice(i,1)):i++;return l}function Ut(t){return[...t].reduce((e,[l,i])=>(e[l]=i,e),{})}function At(t){return Object.keys(t).map(e=>[e,t[e]])}function $l(t,e){return String.fromCharCode(t.charCodeAt(0)+e)}function ce(t){return t.x!=null&&t.y!=null}class g{constructor(e,l){this.x=0,this.y=0,this.set(e,l)}set(e=0,l=0){return ce(e)?(this.x=e.x,this.y=e.y,this):(this.x=e,this.y=l,this)}add(e,l){return ce(e)?(this.x+=e.x,this.y+=e.y,this):(this.x+=e,this.y+=l,this)}sub(e,l){return ce(e)?(this.x-=e.x,this.y-=e.y,this):(this.x-=e,this.y-=l,this)}mul(e){return this.x*=e,this.y*=e,this}div(e){return this.x/=e,this.y/=e,this}clamp(e,l,i,s){return this.x=P(this.x,e,l),this.y=P(this.y,i,s),this}wrap(e,l,i,s){return this.x=B(this.x,e,l),this.y=B(this.y,i,s),this}addWithAngle(e,l){return this.x+=Math.cos(e)*l,this.y+=Math.sin(e)*l,this}swapXy(){const e=this.x;return this.x=this.y,this.y=e,this}normalize(){return this.div(this.length),this}rotate(e){if(e===0)return this;const l=this.x;return this.x=l*Math.cos(e)-this.y*Math.sin(e),this.y=l*Math.sin(e)+this.y*Math.cos(e),this}angleTo(e,l){return ce(e)?Math.atan2(e.y-this.y,e.x-this.x):Math.atan2(l-this.y,e-this.x)}distanceTo(e,l){let i,s;return ce(e)?(i=e.x-this.x,s=e.y-this.y):(i=e-this.x,s=l-this.y),Math.sqrt(i*i+s*s)}isInRect(e,l,i,s){return E(this.x,e,e+i)&&E(this.y,l,l+s)}equals(e){return this.x===e.x&&this.y===e.y}floor(){return this.x=Math.floor(this.x),this.y=Math.floor(this.y),this}round(){return this.x=Math.round(this.x),this.y=Math.round(this.y),this}ceil(){return this.x=Math.ceil(this.x),this.y=Math.ceil(this.y),this}get length(){return Math.sqrt(this.x*this.x+this.y*this.y)}get angle(){return Math.atan2(this.y,this.x)}}const Jt=["transparent","white","red","green","yellow","blue","purple","cyan","black","light_red","light_green","light_yellow","light_blue","light_purple","light_cyan","light_black"],Bl="twrgybpclRGYBPCL";let ue;const Ll=[15658734,15277667,5025616,16761095,4149685,10233776,240116,6381921];function Tl(t){const[e,l,i]=tt(0,t);if(ue=Ut(Jt.map((s,r)=>{if(r<1)return[s,{r:0,g:0,b:0,a:0}];if(r<9){const[c,h,m]=tt(r-1,t);return[s,{r:c,g:h,b:m,a:1}]}const[o,a,u]=tt(r-9+1,t);return[s,{r:Math.floor(t?o*.5:e-(e-o)*.5),g:Math.floor(t?a*.5:i-(i-a)*.5),b:Math.floor(t?u*.5:l-(l-u)*.5),a:1}]})),t){const s=ue.blue;ue.white={r:Math.floor(s.r*.15),g:Math.floor(s.g*.15),b:Math.floor(s.b*.15),a:1}}}function tt(t,e){e&&(t===0?t=7:t===7&&(t=0));const l=Ll[t];return[(l&16711680)>>16,(l&65280)>>8,l&255]}function ie(t,e=1){const l=ue[t];return Math.floor(l.r*e)<<16|Math.floor(l.g*e)<<8|Math.floor(l.b*e)}function de(t,e=1){const l=ue[t],i=Math.floor(l.r*e),s=Math.floor(l.g*e),r=Math.floor(l.b*e);return l.a<1?`rgba(${i},${s},${r},${l.a})`:`rgb(${i},${s},${r})`}const Ul=`
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
`;function Al(t,e){return new PIXI.Filter(void 0,Ul,{width:t,height:e})}const D=new g;let S,L,y,b=new g;const Kt=5;document.createElement("img");let w,fe,he=1,lt="black",x,Nt,ne=!1,p,_t;function Jl(t,e,l,i,s,r,o){D.set(t),p=o,lt=l;const a=`
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
`;if(document.body.style.cssText=a,b.set(D),p.isUsingPixi){b.mul(Kt);const m=new PIXI.Application({width:b.x,height:b.y});if(S=m.view,y=new PIXI.Graphics,y.scale.x=y.scale.y=Kt,PIXI.settings.SCALE_MODE=PIXI.SCALE_MODES.NEAREST,m.stage.addChild(y),y.filters=[],p.name==="crt"&&y.filters.push(_t=new PIXI.filters.CRTFilter({vignettingAlpha:.7})),p.name==="pixel"&&y.filters.push(Al(b.x,b.y)),p.name==="pixel"||p.name==="shapeDark"){const v=new PIXI.filters.AdvancedBloomFilter({threshold:.1,bloomScale:p.name==="pixel"?1.5:1,brightness:p.name==="pixel"?1.5:1,blur:8});y.filters.push(v)}y.lineStyle(0),S.style.cssText=u}else S=document.createElement("canvas"),S.width=b.x,S.height=b.y,L=S.getContext("2d"),L.imageSmoothingEnabled=!1,S.style.cssText=u+c;document.body.appendChild(S);const h=()=>{const v=innerWidth/innerHeight,j=b.x/b.y,X=v<j,z=X?.95*innerWidth:.95*innerHeight*j,Ze=X?.95*innerWidth/j:.95*innerHeight;S.style.width=`${z}px`,S.style.height=`${Ze}px`};if(window.addEventListener("resize",h),h(),i){w=document.createElement("canvas");let m;s?(w.width=b.x,w.height=b.y,m=r):(b.x<=b.y*2?(w.width=b.y*2,w.height=b.y):(w.width=b.x,w.height=b.x/2),w.width>400&&(he=400/w.width,w.width=400,w.height*=he),m=Math.round(400/w.width)),fe=w.getContext("2d"),fe.fillStyle=e,gcc.setOptions({scale:m,capturingFps:60,isSmoothingEnabled:!1})}}function Ee(){if(p.isUsingPixi){y.clear(),y.beginFill(ie(lt,p.isDarkColor?.15:1)),y.drawRect(0,0,D.x,D.y),y.endFill(),y.beginFill(ie(x)),ne=!0;return}L.fillStyle=de(lt,p.isDarkColor?.15:1),L.fillRect(0,0,D.x,D.y),L.fillStyle=de(x)}function T(t){if(t===x){p.isUsingPixi&&!ne&&Ie(ie(x));return}if(x=t,p.isUsingPixi){ne&&y.endFill(),Ie(ie(x));return}L.fillStyle=de(t)}function Ie(t){De(),y.beginFill(t),ne=!0}function De(){ne&&(y.endFill(),ne=!1)}function Fe(){Nt=x}function $e(){T(Nt)}function ge(t,e,l,i){if(p.isUsingPixi){p.name==="shape"||p.name==="shapeDark"?y.drawRoundedRect(t,e,l,i,2):y.drawRect(t,e,l,i);return}L.fillRect(t,e,l,i)}function Kl(t,e,l,i,s){const r=ie(x);Ie(r),y.drawCircle(t,e,s*.5),y.drawCircle(l,i,s*.5),De(),y.lineStyle(s,r),y.moveTo(t,e),y.lineTo(l,i),y.lineStyle(0)}function Nl(){_t.time+=.2}function _l(){if(fe.fillRect(0,0,w.width,w.height),he===1)fe.drawImage(S,(w.width-S.width)/2,(w.height-S.height)/2);else{const t=S.width*he,e=S.height*he;fe.drawImage(S,(w.width-t)/2,(w.height-e)/2,t,e)}gcc.capture(w)}const Ht=[`
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

`];let se,Be;function Hl(){se=[],Be=[]}function Wt(){se=se.concat(Be),Be=[]}function Xt(t){let e={isColliding:{rect:{},text:{},char:{}}};return se.forEach(l=>{Wl(t,l)&&(e=Object.assign(Object.assign(Object.assign({},e),it(l.collision.isColliding.rect)),{isColliding:{rect:Object.assign(Object.assign({},e.isColliding.rect),l.collision.isColliding.rect),text:Object.assign(Object.assign({},e.isColliding.text),l.collision.isColliding.text),char:Object.assign(Object.assign({},e.isColliding.char),l.collision.isColliding.char)}}))}),e}function Wl(t,e){const l=e.pos.x-t.pos.x,i=e.pos.y-t.pos.y;return-e.size.x<l&&l<t.size.x&&-e.size.y<i&&i<t.size.y}function it(t){if(t==null)return{};const e={transparent:"tr",white:"wh",red:"rd",green:"gr",yellow:"yl",blue:"bl",purple:"pr",cyan:"cy",black:"lc"};let l={};return At(t).forEach(([i,s])=>{const r=e[i];s&&r!=null&&(l[r]=!0)}),l}function qt(t,e,l,i){return Vt(!1,t,e,l,i)}function Xl(t,e,l,i){return Vt(!0,t,e,l,i)}function Vt(t,e,l,i,s){if(typeof l=="number"){if(typeof i=="number")return ot(e,l,i,Object.assign({isCharacter:t,isCheckingCollision:!0,color:x},s));throw"invalid params"}else return ot(e,l.x,l.y,Object.assign({isCharacter:t,isCheckingCollision:!0,color:x},i))}const pe=6,me=1,d=pe*me;let Yt,nt,st,rt=!1,H,q,ye,Le;const we={color:"black",backgroundColor:"transparent",rotation:0,mirror:{x:1,y:1},scale:{x:1,y:1},isCharacter:!1,isCheckingCollision:!1};function ql(){H=document.createElement("canvas"),H.width=H.height=d,q=H.getContext("2d"),ye=document.createElement("canvas"),Le=ye.getContext("2d"),Yt=Ht.map((t,e)=>Object.assign(Object.assign({},ct(t)),{hitBox:Te(String.fromCharCode(33+e),!1)})),nt=Ht.map((t,e)=>Object.assign(Object.assign({},ct(t)),{hitBox:Te(String.fromCharCode(33+e),!0)})),st={}}function Vl(t,e){const l=e.charCodeAt(0)-33;t.forEach((i,s)=>{nt[l+s]=Object.assign(Object.assign({},ct(i)),{hitBox:Te(String.fromCharCode(33+l+s),!0)})})}function Yl(){rt=!0}function ot(t,e,l,i={}){const s=tl(i);e-=d/2*s.scale.x,l-=d/2*s.scale.y;const r=Math.floor(e);let o=t,a=r,u=Math.floor(l),c={isColliding:{rect:{},text:{},char:{}}};for(let h=0;h<o.length;h++){const m=o[h];if(m===`
`){a=r,u+=d*s.scale.y;continue}const v=Qt(m,a,u,s);s.isCheckingCollision&&(c={isColliding:{rect:Object.assign(Object.assign({},c.isColliding.rect),v.isColliding.rect),text:Object.assign(Object.assign({},c.isColliding.text),v.isColliding.text),char:Object.assign(Object.assign({},c.isColliding.char),v.isColliding.char)}}),a+=d*s.scale.x}return c}function Qt(t,e,l,i){const s=t.charCodeAt(0);if(s<32||s>126)return{isColliding:{rect:{},text:{},char:{}}};const r=tl(i);if(r.backgroundColor!=="transparent"&&(Fe(),T(r.backgroundColor),ge(e,l,d*r.scale.x,d*r.scale.y),$e()),s<=32)return{isColliding:{rect:{},text:{},char:{}}};const o=s-33,a=r.isCharacter?nt[o]:Yt[o],u=B(r.rotation,0,4);if(r.color==="black"&&u===0&&r.mirror.x===1&&r.mirror.y===1&&(!p.isUsingPixi||r.scale.x===1&&r.scale.y===1))return at(a,e,l,r.scale,r.isCheckingCollision,!0);const c=JSON.stringify({c:t,options:r}),h=st[c];if(h!=null)return at(h,e,l,r.scale,r.isCheckingCollision,r.color!=="transparent");let m=!1;p.isUsingPixi&&(r.scale.x!==1||r.scale.y!==1)&&(ye.width=d*r.scale.x,ye.height=d*r.scale.y,Le.imageSmoothingEnabled=!1,Le.scale(r.scale.x,r.scale.y),Zt(Le,u,r,a),m=!0),q.clearRect(0,0,d,d),Zt(q,u,r,a);const v=Te(t,r.isCharacter);let j;if(rt||p.isUsingPixi){const X=document.createElement("img");if(X.src=H.toDataURL(),p.isUsingPixi){const z=document.createElement("img");z.src=(m?ye:H).toDataURL(),j=PIXI.Texture.from(z)}rt&&(st[c]={image:X,texture:j,hitBox:v})}return at({image:H,texture:j,hitBox:v},e,l,r.scale,r.isCheckingCollision,r.color!=="transparent")}function Zt(t,e,l,i){e===0&&l.mirror.x===1&&l.mirror.y===1?t.drawImage(i.image,0,0):(t.save(),t.translate(d/2,d/2),t.rotate(Math.PI/2*e),(l.mirror.x===-1||l.mirror.y===-1)&&t.scale(l.mirror.x,l.mirror.y),t.drawImage(i.image,-d/2,-d/2),t.restore()),l.color!=="black"&&(t.globalCompositeOperation="source-in",t.fillStyle=de(l.color==="transparent"?"black":l.color),t.fillRect(0,0,d,d),t.globalCompositeOperation="source-over")}function at(t,e,l,i,s,r){if(r&&(i.x===1&&i.y===1?el(t,e,l):el(t,e,l,d*i.x,d*i.y)),!s)return;const o={pos:{x:e+t.hitBox.pos.x*i.x,y:l+t.hitBox.pos.y*i.y},size:{x:t.hitBox.size.x*i.x,y:t.hitBox.size.y*i.y},collision:t.hitBox.collision},a=Xt(o);return r&&se.push(o),a}function el(t,e,l,i,s){if(p.isUsingPixi){De(),y.beginTextureFill({texture:t.texture,matrix:new PIXI.Matrix().translate(e,l)}),y.drawRect(e,l,i==null?d:i,s==null?d:s),Ie(ie(x));return}i==null?L.drawImage(t.image,e,l):L.drawImage(t.image,e,l,i,s)}function ct(t,e=!0){q.clearRect(0,0,d,d);let l=t.split(`
`);e&&(l=l.slice(1,l.length-1));let i=0;l.forEach(u=>{i=Math.max(u.length,i)});const s=Math.max(Math.ceil((pe-i)/2),0),r=l.length,o=Math.max(Math.ceil((pe-r)/2),0);l.forEach((u,c)=>{if(!(c+o>=pe))for(let h=0;h<pe-s;h++){const m=u.charAt(h);let v=Bl.indexOf(m);m!==""&&v>=1&&(q.fillStyle=de(Jt[v]),q.fillRect((h+s)*me,(c+o)*me,me,me))}});const a=document.createElement("img");return a.src=H.toDataURL(),p.isUsingPixi?{image:a,texture:PIXI.Texture.from(a)}:{image:a}}function Te(t,e){const l={pos:new g(d,d),size:new g,collision:{isColliding:{char:{},text:{}}}};e?l.collision.isColliding.char[t]=!0:l.collision.isColliding.text[t]=!0;const i=q.getImageData(0,0,d,d).data;let s=0;for(let r=0;r<d;r++)for(let o=0;o<d;o++)i[s+3]>0&&(o<l.pos.x&&(l.pos.x=o),r<l.pos.y&&(l.pos.y=r)),s+=4;s=0;for(let r=0;r<d;r++)for(let o=0;o<d;o++)i[s+3]>0&&(o>l.pos.x+l.size.x-1&&(l.size.x=o-l.pos.x+1),r>l.pos.y+l.size.y-1&&(l.size.y=r-l.pos.y+1)),s+=4;return l}function tl(t){let e=Object.assign(Object.assign({},we),t);return t.scale!=null&&(e.scale=Object.assign(Object.assign({},we.scale),t.scale)),t.mirror!=null&&(e.mirror=Object.assign(Object.assign({},we.mirror),t.mirror)),e}let re=!1,Ue=!1,ut=!1;const ll=["Escape","Digit0","Digit1","Digit2","Digit3","Digit4","Digit5","Digit6","Digit7","Digit8","Digit9","Minus","Equal","Backspace","Tab","KeyQ","KeyW","KeyE","KeyR","KeyT","KeyY","KeyU","KeyI","KeyO","KeyP","BracketLeft","BracketRight","Enter","ControlLeft","KeyA","KeyS","KeyD","KeyF","KeyG","KeyH","KeyJ","KeyK","KeyL","Semicolon","Quote","Backquote","ShiftLeft","Backslash","KeyZ","KeyX","KeyC","KeyV","KeyB","KeyN","KeyM","Comma","Period","Slash","ShiftRight","NumpadMultiply","AltLeft","Space","CapsLock","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","Pause","ScrollLock","Numpad7","Numpad8","Numpad9","NumpadSubtract","Numpad4","Numpad5","Numpad6","NumpadAdd","Numpad1","Numpad2","Numpad3","Numpad0","NumpadDecimal","IntlBackslash","F11","F12","F13","F14","F15","F16","F17","F18","F19","F20","F21","F22","F23","F24","IntlYen","Undo","Paste","MediaTrackPrevious","Cut","Copy","MediaTrackNext","NumpadEnter","ControlRight","LaunchMail","AudioVolumeMute","MediaPlayPause","MediaStop","Eject","AudioVolumeDown","AudioVolumeUp","BrowserHome","NumpadDivide","PrintScreen","AltRight","Help","NumLock","Pause","Home","ArrowUp","PageUp","ArrowLeft","ArrowRight","End","ArrowDown","PageDown","Insert","Delete","OSLeft","OSRight","ContextMenu","BrowserSearch","BrowserFavorites","BrowserRefresh","BrowserStop","BrowserForward","BrowserBack"];let dt;const Ql={onKeyDown:void 0};let ft,ht=!1,gt=!1,pt=!1,mt={},yt={},wt={};function il(t){ft=Object.assign(Object.assign({},Ql),t),dt=Ut(ll.map(e=>[e,{isPressed:!1,isJustPressed:!1,isJustReleased:!1}])),document.addEventListener("keydown",e=>{ht=gt=!0,mt[e.code]=yt[e.code]=!0,ft.onKeyDown!=null&&ft.onKeyDown(),(e.code==="AltLeft"||e.code==="AltRight"||e.code==="ArrowRight"||e.code==="ArrowDown"||e.code==="ArrowLeft"||e.code==="ArrowUp")&&e.preventDefault()}),document.addEventListener("keyup",e=>{ht=!1,pt=!0,mt[e.code]=!1,wt[e.code]=!0})}function nl(){Ue=!re&&gt,ut=re&&pt,gt=pt=!1,re=ht,At(dt).forEach(([t,e])=>{e.isJustPressed=!e.isPressed&&yt[t],e.isJustReleased=e.isPressed&&wt[t],e.isPressed=!!mt[t]}),yt={},wt={}}function sl(){Ue=!1,re=!0}var Zl=Object.freeze({__proto__:null,get isPressed(){return re},get isJustPressed(){return Ue},get isJustReleased(){return ut},codes:ll,get code(){return dt},init:il,update:nl,clearJustPressed:sl});class Ae{constructor(e=null){this.setSeed(e)}get(e=1,l){return l==null&&(l=e,e=0),this.next()/4294967295*(l-e)+e}getInt(e,l){l==null&&(l=e,e=0);const i=Math.floor(e),s=Math.floor(l);return s===i?i:this.next()%(s-i)+i}getPlusOrMinus(){return this.getInt(2)*2-1}select(e){return e[this.getInt(e.length)]}setSeed(e,l=123456789,i=362436069,s=521288629,r=32){this.w=e!=null?e>>>0:Math.floor(Math.random()*4294967295)>>>0,this.x=l>>>0,this.y=i>>>0,this.z=s>>>0;for(let o=0;o<r;o++)this.next();return this}getState(){return{x:this.x,y:this.y,z:this.z,w:this.w}}next(){const e=this.x^this.x<<11;return this.x=this.y,this.y=this.z,this.z=this.w,this.w=(this.w^this.w>>>19^(e^e>>>8))>>>0,this.w}}const be=new g;let U=!1,oe=!1,Ce=!1,ei={isDebugMode:!1,anchor:new g,padding:new g,onPointerDownOrUp:void 0},O,R,M;const Se=new Ae,V=new g,A=new g;let Me=!1,ve=new g,bt=!1,Ct=!1,St=!1;function rl(t,e,l){M=Object.assign(Object.assign({},ei),l),O=t,R=new g(e.x+M.padding.x*2,e.y+M.padding.y*2),ve.set(O.offsetLeft+O.clientWidth*(.5-M.anchor.x),O.offsetTop+O.clientWidth*(.5-M.anchor.y)),M.isDebugMode&&V.set(O.offsetLeft+O.clientWidth*(.5-M.anchor.x),O.offsetTop+O.clientWidth*(.5-M.anchor.y)),document.addEventListener("mousedown",i=>{cl(i.pageX,i.pageY)}),document.addEventListener("touchstart",i=>{cl(i.touches[0].pageX,i.touches[0].pageY)}),document.addEventListener("mousemove",i=>{ul(i.pageX,i.pageY)}),document.addEventListener("touchmove",i=>{i.preventDefault(),ul(i.touches[0].pageX,i.touches[0].pageY)},{passive:!1}),document.addEventListener("mouseup",i=>{dl()}),document.addEventListener("touchend",i=>{i.preventDefault(),i.target.click(),dl()},{passive:!1})}function ol(){ti(ve.x,ve.y,be),M.isDebugMode&&!be.isInRect(0,0,R.x,R.y)?(li(),be.set(V),oe=!U&&Me,Ce=U&&!Me,U=Me):(oe=!U&&Ct,Ce=U&&St,U=bt),Ct=St=!1}function al(){oe=!1,U=!0}function ti(t,e,l){O!=null&&(l.x=Math.round(((t-O.offsetLeft)/O.clientWidth+M.anchor.x)*R.x-M.padding.x),l.y=Math.round(((e-O.offsetTop)/O.clientHeight+M.anchor.y)*R.y-M.padding.y))}function li(){A.length>0?(V.add(A),!E(V.x,-R.x*.1,R.x*1.1)&&V.x*A.x>0&&(A.x*=-1),!E(V.y,-R.y*.1,R.y*1.1)&&V.y*A.y>0&&(A.y*=-1),Se.get()<.05&&A.set(0)):Se.get()<.1&&(A.set(0),A.addWithAngle(Se.get(Math.PI*2),(R.x+R.y)*Se.get(.01,.03))),Se.get()<.05&&(Me=!Me)}function cl(t,e){ve.set(t,e),bt=Ct=!0,M.onPointerDownOrUp!=null&&M.onPointerDownOrUp()}function ul(t,e){ve.set(t,e)}function dl(t){bt=!1,St=!0,M.onPointerDownOrUp!=null&&M.onPointerDownOrUp()}var ii=Object.freeze({__proto__:null,pos:be,get isPressed(){return U},get isJustPressed(){return oe},get isJustReleased(){return Ce},init:rl,update:ol,clearJustPressed:al});let J=new g,K=!1,F=!1,W=!1;function fl(t){il({onKeyDown:t}),rl(S,D,{onPointerDownOrUp:t,anchor:new g(.5,.5)})}function hl(){nl(),ol(),J=be,K=re||U,F=Ue||oe,W=ut||Ce}function gl(){sl(),al()}function xe(t){J.set(t.pos),K=t.isPressed,F=t.isJustPressed,W=t.isJustReleased}var ni=Object.freeze({__proto__:null,get pos(){return J},get isPressed(){return K},get isJustPressed(){return F},get isJustReleased(){return W},init:fl,update:hl,clearJustPressed:gl,set:xe});let pl,ml;const yl=68,Mt=1e3/yl;let Pe=0;const si={viewSize:{x:126,y:126},bodyBackground:"#111",viewBackground:"black",isUsingVirtualPad:!0,isFourWaysStick:!1,isCapturing:!1,isCapturingGameCanvasOnly:!1,isSoundEnabled:!0,captureCanvasScale:1,theme:{name:"simple",isUsingPixi:!1,isDarkColor:!1}};let G,wl=10;function ri(t,e,l){pl=t,ml=e,G=Object.assign(Object.assign({},si),l),Tl(G.theme.isDarkColor),Jl(G.viewSize,G.bodyBackground,G.viewBackground,G.isCapturing,G.isCapturingGameCanvasOnly,G.captureCanvasScale,G.theme),fl(G.isSoundEnabled?sss.startAudio:()=>{}),ql(),pl(),bl()}function bl(){requestAnimationFrame(bl);const t=window.performance.now();t<Pe-yl/12||(Pe+=Mt,(Pe<t||Pe>t+Mt*2)&&(Pe=t+Mt),G.isSoundEnabled&&sss.update(),hl(),ml(),G.isCapturing&&_l(),wl--,wl===0&&Yl())}class oi{constructor(e){this.size=new g,this.size.set(e),this.letterGrid=f(this.size.x).map(()=>f(this.size.y).map(()=>{})),this.colorGrid=f(this.size.x).map(()=>f(this.size.y).map(()=>{})),this.backgroundColorGrid=f(this.size.x).map(()=>f(this.size.y).map(()=>{})),this.rotationGrid=f(this.size.x).map(()=>f(this.size.y).map(()=>{})),this.characterGrid=f(this.size.x).map(()=>f(this.size.y).map(()=>{}))}print(e,l,i,s={}){const r=Object.assign(Object.assign({},we),s);let o=Math.floor(l),a=Math.floor(i);const u=o;for(let c=0;c<e.length;c++){const h=e[c];if(h===`
`){o=u,a++;continue}if(o<0||o>=this.size.x||a<0||a>=this.size.y){o++;continue}this.letterGrid[o][a]=h,this.colorGrid[o][a]=r.color,this.backgroundColorGrid[o][a]=r.backgroundColor,this.rotationGrid[o][a]=r.rotation,this.characterGrid[o][a]=r.isCharacter,o++}}getCharAt(e,l){if(e<0||e>=this.size.x||l<0||l>=this.size.y)return;const i=Math.floor(e),s=Math.floor(l),r=this.letterGrid[i][s],o=this.colorGrid[i][s],a=this.backgroundColorGrid[i][s],u=this.rotationGrid[i][s],c=this.characterGrid[i][s];return{char:r,options:{color:o,backgroundColor:a,rotation:u,isCharacter:c}}}setCharAt(e,l,i,s){if(e<0||e>=this.size.x||l<0||l>=this.size.y)return;const r=Object.assign(Object.assign({},we),s),o=Math.floor(e),a=Math.floor(l);this.letterGrid[o][a]=i,this.colorGrid[o][a]=r.color,this.backgroundColorGrid[o][a]=r.backgroundColor,this.rotationGrid[o][a]=r.rotation,this.characterGrid[o][a]=r.isCharacter}draw(){for(let e=0;e<this.size.x;e++)for(let l=0;l<this.size.y;l++){const i=this.letterGrid[e][l];if(i==null)continue;const s=this.colorGrid[e][l],r=this.backgroundColorGrid[e][l],o=this.rotationGrid[e][l],a=this.characterGrid[e][l];Qt(i,e*d,l*d,{color:s,backgroundColor:r,rotation:o,isCharacter:a})}}clear(){for(let e=0;e<this.size.x;e++)for(let l=0;l<this.size.y;l++)this.letterGrid[e][l]=this.colorGrid[e][l]=this.backgroundColorGrid[e][l]=this.rotationGrid[e][l]=this.characterGrid[e][l]=void 0}scrollUp(){for(let l=0;l<this.size.x;l++)for(let i=1;i<this.size.y;i++)this.letterGrid[l][i-1]=this.letterGrid[l][i],this.colorGrid[l][i-1]=this.colorGrid[l][i],this.backgroundColorGrid[l][i-1]=this.backgroundColorGrid[l][i],this.rotationGrid[l][i-1]=this.rotationGrid[l][i],this.characterGrid[l][i-1]=this.characterGrid[l][i];const e=this.size.y-1;for(let l=0;l<this.size.x;l++)this.letterGrid[l][e]=this.colorGrid[l][e]=this.backgroundColorGrid[l][e]=this.rotationGrid[l][e]=this.characterGrid[l][e]=void 0}getState(){return{charGrid:this.letterGrid.map(e=>[].concat(e)),colorGrid:this.colorGrid.map(e=>[].concat(e)),backgroundColorGrid:this.backgroundColorGrid.map(e=>[].concat(e)),rotationGrid:this.rotationGrid.map(e=>[].concat(e)),symbolGrid:this.characterGrid.map(e=>[].concat(e))}}setState(e){this.letterGrid=e.charGrid.map(l=>[].concat(l)),this.colorGrid=e.colorGrid.map(l=>[].concat(l)),this.backgroundColorGrid=e.backgroundColorGrid.map(l=>[].concat(l)),this.rotationGrid=e.rotationGrid.map(l=>[].concat(l)),this.characterGrid=e.symbolGrid.map(l=>[].concat(l))}}let Je;const Ke=new Ae;function vt(){Je=[]}function Cl(t,e=16,l=1,i=0,s=Math.PI*2){if(e<1){if(Ke.get()>e)return;e=1}for(let r=0;r<e;r++){const o=i+Ke.get(s)-s/2,a={pos:new g(t),vel:new g(l*Ke.get(.5,1),0).rotate(o),color:x,ticks:P(Ke.get(10,20)*Math.sqrt(Math.abs(l)),10,60)};Je.push(a)}}function Ne(){Fe(),Je=Je.filter(t=>(t.ticks--,t.ticks<0?!1:(t.pos.add(t.vel),t.vel.mul(.98),T(t.color),ge(Math.floor(t.pos.x),Math.floor(t.pos.y),1,1),!0))),$e()}function xt(t,e,l,i){return Sl(!1,t,e,l,i)}function ai(t,e,l,i){return Sl(!0,t,e,l,i)}function ci(t,e,l,i,s=.5,r=.5){typeof t!="number"&&(r=s,s=i,i=l,l=e,e=t.y,t=t.x);const o=new g(l).rotate(s),a=new g(t-o.x*r,e-o.y*r);return Pt(a,o,i)}function ui(t,e,l=3,i=3,s=3){const r=new g,o=new g;if(typeof t=="number")if(typeof e=="number")typeof l=="number"?(r.set(t,e),o.set(l,i)):(r.set(t,e),o.set(l),s=i);else throw"invalid params";else if(typeof e=="number")if(typeof l=="number")r.set(t),o.set(e,l),s=i;else throw"invalid params";else if(typeof l=="number")r.set(t),o.set(e),s=l;else throw"invalid params";return Pt(r,o.sub(r),s)}function di(t,e,l,i,s,r){let o=new g;typeof t=="number"?o.set(t,e):(o.set(t),r=s,s=i,i=l,l=e),i==null&&(i=3),s==null&&(s=0),r==null&&(r=Math.PI*2);let a,u;if(s>r?(a=r,u=s-r):(a=s,u=r-s),u=P(u,0,Math.PI*2),u<.01)return;const c=P(Math.ceil(u*Math.sqrt(l*.25)),1,36),h=u/c;let m=a,v=new g(l).rotate(m).add(o),j=new g,X=new g,z={isColliding:{rect:{},text:{},char:{}}};for(let Ze=0;Ze<c;Ze++){m+=h,j.set(l).rotate(m).add(o),X.set(j).sub(v);const et=Pt(v,X,i,!0);z=Object.assign(Object.assign(Object.assign({},z),it(et.isColliding.rect)),{isColliding:{rect:Object.assign(Object.assign({},z.isColliding.rect),et.isColliding.rect),text:Object.assign(Object.assign({},z.isColliding.text),et.isColliding.text),char:Object.assign(Object.assign({},z.isColliding.char),et.isColliding.char)}}),v.set(j)}return Wt(),z}function Sl(t,e,l,i,s){if(typeof e=="number"){if(typeof l=="number")return typeof i=="number"?s==null?Y(t,e,l,i,i):Y(t,e,l,i,s):Y(t,e,l,i.x,i.y);throw"invalid params"}else if(typeof l=="number"){if(i==null)return Y(t,e.x,e.y,l,l);if(typeof i=="number")return Y(t,e.x,e.y,l,i);throw"invalid params"}else return Y(t,e.x,e.y,l.x,l.y)}function Pt(t,e,l,i=!1){let s=!0;(p.name==="shape"||p.name==="shapeDark")&&(x!=="transparent"&&Kl(t.x,t.y,t.x+e.x,t.y+e.y,l),s=!1);const r=Math.floor(P(l,3,10)),o=Math.abs(e.x),a=Math.abs(e.y),u=P(Math.ceil(o>a?o/r:a/r)+1,3,99);e.div(u-1);let c={isColliding:{rect:{},text:{},char:{}}};for(let h=0;h<u;h++){const m=Y(!0,t.x,t.y,l,l,!0,s);c=Object.assign(Object.assign(Object.assign({},c),it(m.isColliding.rect)),{isColliding:{rect:Object.assign(Object.assign({},c.isColliding.rect),m.isColliding.rect),text:Object.assign(Object.assign({},c.isColliding.text),m.isColliding.text),char:Object.assign(Object.assign({},c.isColliding.char),m.isColliding.char)}}),t.add(e)}return i||Wt(),c}function Y(t,e,l,i,s,r=!1,o=!0){let a=o;(p.name==="shape"||p.name==="shapeDark")&&a&&x!=="transparent"&&(t?ge(e-i/2,l-s/2,i,s):ge(e,l,i,s),a=!1);let u=t?{x:Math.floor(e-i/2),y:Math.floor(l-s/2)}:{x:Math.floor(e),y:Math.floor(l)};const c={x:Math.trunc(i),y:Math.trunc(s)};if(c.x===0||c.y===0)return{isColliding:{rect:{},text:{},char:{}}};c.x<0&&(u.x+=c.x,c.x*=-1),c.y<0&&(u.y+=c.y,c.y*=-1);const h={pos:u,size:c,collision:{isColliding:{rect:{}}}};h.collision.isColliding.rect[x]=!0;const m=Xt(h);return x!=="transparent"&&((r?Be:se).push(h),a&&ge(u.x,u.y,c.x,c.y)),m}function Ot({pos:t,size:e,text:l,isToggle:i=!1,onClick:s=()=>{}}){return{pos:t,size:e,text:l,isToggle:i,onClick:s,isPressed:!1,isSelected:!1,isHovered:!1,toggleGroup:[]}}function kt(t){const e=new g(J).sub(t.pos);t.isHovered=e.isInRect(0,0,t.size.x,t.size.y),t.isHovered&&oe&&(t.isPressed=!0),t.isPressed&&!t.isHovered&&(t.isPressed=!1),t.isPressed&&Ce&&(t.onClick(),t.isPressed=!1,t.isToggle&&(t.toggleGroup.length===0?t.isSelected=!t.isSelected:(t.toggleGroup.forEach(l=>{l.isSelected=!1}),t.isSelected=!0))),_e(t)}function _e(t){Fe(),T(t.isPressed?"blue":"light_blue"),xt(t.pos.x,t.pos.y,t.size.x,t.size.y),t.isToggle&&!t.isSelected&&(T("white"),xt(t.pos.x+1,t.pos.y+1,t.size.x-2,t.size.y-2)),T(t.isHovered?"black":"blue"),qt(t.text,t.pos.x+3,t.pos.y+3),$e()}let $,Oe,Q,Gt;function fi(t){$={randomSeed:t,inputs:[]}}function hi(t){$.inputs.push(t)}function Ml(){return $!=null}function gi(t){Oe=0,t.setSeed($.randomSeed)}function pi(){Oe>=$.inputs.length||(xe($.inputs[Oe]),Oe++)}function mi(){Q=[]}function yi(t,e,l){Q.push({randomState:l.getState(),gameState:cloneDeep(t),baseState:cloneDeep(e)})}function wi(t){const e=Q.pop(),l=e.randomState;return t.setSeed(l.w,l.x,l.y,l.z,0),Gt={pos:new g(J),isPressed:K,isJustPressed:F,isJustReleased:W},xe($.inputs.pop()),e}function bi(t){const e=Q[Q.length-1],l=e.randomState;return t.setSeed(l.w,l.x,l.y,l.z,0),Gt={pos:new g(J),isPressed:K,isJustPressed:F,isJustReleased:W},xe($.inputs[$.inputs.length-1]),e}function Ci(){xe(Gt)}function Si(){return Q.length===0}function Mi(){const t=Oe-1;if(!(t>=$.inputs.length))return Q[t]}const vi=Math.PI,xi=Math.abs,Pi=Math.sin,Oi=Math.cos,ki=Math.atan2,Gi=Math.sqrt,Ri=Math.pow,ji=Math.floor,zi=Math.round,Ei=Math.ceil;n.ticks=0,n.difficulty=void 0,n.score=0,n.time=void 0,n.isReplaying=!1;function Ii(t=1,e){return N.get(t,e)}function Di(t=2,e){return N.getInt(t,e)}function Fi(t=1,e){return N.get(t,e)*N.getPlusOrMinus()}function Rt(t="GAME OVER"){Qe=t,ae&&(n.time=void 0),jl()}function $i(t="COMPLETE"){Qe=t,jl()}function Bi(t,e,l){if(n.isReplaying||(n.score+=t,e==null))return;const i=`${t>=1?"+":""}${Math.floor(t)}`;let s=new g;typeof e=="number"?s.set(e,l):s.set(e),s.x-=i.length*d/2,s.y-=d/2,Ve.push({str:i,pos:s,vy:-2,ticks:30})}function vl(t){T(t)}function Li(t,e,l,i,s,r){let o=new g;typeof t=="number"?(o.set(t,e),Cl(o,l,i,s,r)):(o.set(t),Cl(o,e,l,i,s))}function xl(t,e){return new g(t,e)}function Pl(t,e){!je&&!le&&te&&(e!=null&&typeof sss.playSoundEffect=="function"?sss.playSoundEffect(t,e):sss.play(Ai[t]))}let jt;function zt(){typeof sss.generateMml=="function"?jt=sss.playMml(sss.generateMml()):sss.playBgm()}function Et(){jt!=null&&sss.stopMml(jt),sss.stopBgm()}function Ti(t){if(je){const e=bi(N),l=e.baseState;return n.score=l.score,n.ticks=l.ticks,cloneDeep(e.gameState)}else if(le){const e=wi(N),l=e.baseState;return n.score=l.score,n.ticks=l.ticks,e.gameState}else{if(n.isReplaying)return Mi().gameState;if(Z==="inGame"){const e={score:n.score,ticks:n.ticks};yi(t,e,N)}}return t}function Ui(){le||(!n.isReplaying&&qe?Yi():Rt())}const Ai={coin:"c",laser:"l",explosion:"e",powerUp:"p",hit:"h",jump:"j",select:"s",lucky:"u",random:"r",click:"i",synth:"y",tone:"t"},Ol={isPlayingBgm:!1,isCapturing:!1,isCapturingGameCanvasOnly:!1,captureCanvasScale:1,isShowingScore:!0,isShowingTime:!1,isReplayEnabled:!1,isRewindEnabled:!1,isDrawingParticleFront:!1,isDrawingScoreFront:!1,isMinifying:!1,isSoundEnabled:!0,viewSize:{x:100,y:100},seed:0,theme:"simple"},Ji=new Ae,N=new Ae;let Z,Ki={title:Xi,inGame:Wi,gameOver:qi,rewind:Qi},k,It=0,He,We=!0,Xe=0,ee,ke,kl,ae,Ge,qe,Re,Dt,te,_,Ve,je=!1,le=!1,ze,Ye,Qe,Ft;function Ni(t){const e=window;e.update=t.update,e.title=t.title,e.description=t.description,e.characters=t.characters,e.options=t.options,Gl()}function Gl(){let t;typeof options<"u"&&options!=null?t=Object.assign(Object.assign({},Ol),options):t=Ol;const e={name:t.theme,isUsingPixi:!1,isDarkColor:!1};t.theme!=="simple"&&t.theme!=="dark"&&(e.isUsingPixi=!0),(t.theme==="pixel"||t.theme==="shapeDark"||t.theme==="crt"||t.theme==="dark")&&(e.isDarkColor=!0),ee={viewSize:{x:100,y:100},bodyBackground:e.isDarkColor?"#101010":"#e0e0e0",viewBackground:e.isDarkColor?"blue":"white",theme:e,isSoundEnabled:t.isSoundEnabled},Xe=t.seed,ee.isCapturing=t.isCapturing,ee.isCapturingGameCanvasOnly=t.isCapturingGameCanvasOnly,ee.captureCanvasScale=t.captureCanvasScale,ee.viewSize=t.viewSize,ke=t.isPlayingBgm,kl=t.isShowingScore&&!t.isShowingTime,ae=t.isShowingTime,Ge=t.isReplayEnabled,qe=t.isRewindEnabled,Re=t.isDrawingParticleFront,Dt=t.isDrawingScoreFront,te=t.isSoundEnabled,t.isMinifying&&tn(),ri(_i,Hi,ee)}function _i(){typeof description<"u"&&description!=null&&description.trim().length>0&&(We=!1,Xe+=Il(description)),typeof title<"u"&&title!=null&&title.trim().length>0&&(We=!1,document.title=title,Xe+=Il(title)),typeof characters<"u"&&characters!=null&&Vl(characters,"a"),te&&sss.init(Xe);const t=ee.viewSize;_={x:Math.floor(t.x/6),y:Math.floor(t.y/6)},k=new oi(_),T("black"),We?($t(),n.ticks=0):Rl()}function Hi(){n.df=n.difficulty=n.ticks/3600+1,n.tc=n.ticks;const t=n.score,e=n.time;n.sc=n.score;const l=n.sc;n.inp={p:J,ip:K,ijp:F,ijr:W},Hl(),Ki[Z](),p.isUsingPixi&&(De(),p.name==="crt"&&Nl()),n.ticks++,n.isReplaying?(n.score=t,n.time=e):n.sc!==l&&(n.score=n.sc)}function $t(){Z="inGame",n.ticks=-1,vt();const t=Math.floor(n.score);t>It&&(It=t),ae&&n.time!=null&&(He==null||He>n.time)&&(He=n.time),n.score=0,n.time=0,Ve=[],ke&&te&&zt();const e=Ji.getInt(999999999);N.setSeed(e),(Ge||qe)&&(fi(e),mi(),n.isReplaying=!1)}function Wi(){k.clear(),Ee(),Re||Ne(),Dt||El(),(Ge||qe)&&hi({pos:xl(J),isPressed:K,isJustPressed:F,isJustReleased:W}),typeof update=="function"&&update(),Re&&Ne(),Dt&&El(),Bt(),k.draw(),ae&&n.time!=null&&n.time++}function Rl(){Z="title",n.ticks=-1,vt(),k.clear(),Ee(),Ml()&&(gi(N),n.isReplaying=!0)}function Xi(){if(F){$t();return}if(Ee(),Ge&&Ml()&&(pi(),n.inp={p:J,ip:K,ijp:F,ijr:W},Re||Ne(),update(),Re&&Ne()),n.ticks===0&&(Bt(),typeof title<"u"&&title!=null&&k.print(title,Math.floor(_.x-title.length)/2,Math.ceil(_.y*.2))),(n.ticks===30||n.ticks==40)&&typeof description<"u"&&description!=null){let t=0;description.split(`
`).forEach(l=>{l.length>t&&(t=l.length)});const e=Math.floor((_.x-t)/2);description.split(`
`).forEach((l,i)=>{k.print(l,e,Math.floor(_.y/2)+i)})}k.draw()}function jl(){Z="gameOver",n.isReplaying||gl(),n.ticks=-1,Vi(),ke&&te&&Et()}function qi(){(n.isReplaying||n.ticks>20)&&F?$t():n.ticks===(Ge?120:300)&&!We&&Rl()}function Vi(){n.isReplaying||(k.print(Qe,Math.floor((_.x-Qe.length)/2),Math.floor(_.y/2)),k.draw())}function Yi(){Z="rewind",je=!0,ze=Ot({pos:{x:D.x-39,y:11},size:{x:36,y:7},text:"Rewind"}),Ye=Ot({pos:{x:D.x-39,y:D.y-19},size:{x:36,y:7},text:"GiveUp"}),ke&&te&&Et(),p.isUsingPixi&&(_e(ze),_e(Ye))}function Qi(){k.clear(),Ee(),update(),Bt(),Ci(),le?(_e(ze),(Si()||!K)&&Zi()):(kt(ze),kt(Ye),ze.isPressed&&(le=!0,je=!1)),Ye.isPressed?(je=le=!1,Rt()):k.draw(),ae&&n.time!=null&&n.time++}function Zi(){le=!1,Z="inGame",vt(),ke&&te&&zt()}function Bt(){if(kl){k.print(`${Math.floor(n.score)}`,0,0);const t=`HI ${It}`;k.print(t,_.x-t.length,0)}ae&&(zl(n.time,0,0),zl(He,9,0))}function zl(t,e,l){if(t==null)return;let i=Math.floor(t*100/50);i>=10*60*100&&(i=10*60*100-1);const s=Lt(Math.floor(i/6e3),1)+"'"+Lt(Math.floor(i%6e3/100),2)+'"'+Lt(Math.floor(i%100),2);k.print(s,e,l)}function Lt(t,e){return("0000"+t).slice(-e)}function El(){Fe(),T("black"),Ve=Ve.filter(t=>(ot(t.str,t.pos.x,t.pos.y),t.pos.y+=t.vy,t.vy*=.9,t.ticks--,t.ticks>0)),$e()}function Il(t){let e=0;for(let l=0;l<t.length;l++){const i=t.charCodeAt(l);e=(e<<5)-e+i,e|=0}return e}function en(){let t=window.location.search.substring(1);if(t=t.replace(/[^A-Za-z0-9_-]/g,""),t.length===0)return;const e=document.createElement("script");Ft=`${t}/main.js`,e.setAttribute("src",Ft),document.head.appendChild(e)}function tn(){fetch(Ft).then(t=>t.text()).then(t=>{const e=Terser.minify(t+"update();",{toplevel:!0}).code,l="function(){",i=e.indexOf(l),s="options={",r=e.indexOf(s);let o=e;if(i>=0)o=e.substring(e.indexOf(l)+l.length,e.length-4);else if(r>=0){let a=1,u;for(let c=r+s.length;c<e.length;c++){const h=e.charAt(c);if(h==="{")a++;else if(h==="}"&&(a--,a===0)){u=c+2;break}}a===0&&(o=e.substring(u))}Dl.forEach(([a,u])=>{o=o.split(a).join(u)}),console.log(o),console.log(`${o.length} letters`)})}n.inp=void 0;function ln(...t){return vl.apply(this,t)}function nn(...t){return Pl.apply(this,t)}function sn(...t){return C.apply(this,t)}function rn(...t){return I.apply(this.args)}n.tc=void 0,n.df=void 0,n.sc=void 0;const on="transparent",an="white",cn="red",un="green",dn="yellow",fn="blue",hn="purple",gn="cyan",pn="black",mn="coin",yn="laser",wn="explosion",bn="powerUp",Cn="hit",Sn="jump",Mn="select",vn="lucky";let Dl=[["===","=="],["!==","!="],["input.pos","inp.p"],["input.isPressed","inp.ip"],["input.isJustPressed","inp.ijp"],["input.isJustReleased","inp.ijr"],["color(","clr("],["play(","ply("],["times(","tms("],["remove(","rmv("],["ticks","tc"],["difficulty","df"],["score","sc"],[".isColliding.rect.transparent",".tr"],[".isColliding.rect.white",".wh"],[".isColliding.rect.red",".rd"],[".isColliding.rect.green",".gr"],[".isColliding.rect.yellow",".yl"],[".isColliding.rect.blue",".bl"],[".isColliding.rect.purple",".pr"],[".isColliding.rect.cyan",".cy"],[".isColliding.rect.black",".lc"],['"transparent"',"tr"],['"white"',"wh"],['"red"',"rd"],['"green"',"gr"],['"yellow"',"yl"],['"blue"',"bl"],['"purple"',"pr"],['"cyan"',"cy"],['"black"',"lc"],['"coin"',"cn"],['"laser"',"ls"],['"explosion"',"ex"],['"powerUp"',"pw"],['"hit"',"ht"],['"jump"',"jm"],['"select"',"sl"],['"lucky"',"uc"]];n.PI=vi,n.abs=xi,n.addGameScript=en,n.addScore=Bi,n.addWithCharCode=$l,n.arc=di,n.atan2=ki,n.bar=ci,n.bl=fn,n.box=ai,n.ceil=Ei,n.char=Xl,n.clamp=P,n.clr=ln,n.cn=mn,n.color=vl,n.complete=$i,n.cos=Oi,n.cy=gn,n.end=Rt,n.ex=wn,n.floor=ji,n.frameState=Ti,n.getButton=Ot,n.gr=un,n.ht=Cn,n.init=Ni,n.input=ni,n.jm=Sn,n.keyboard=Zl,n.lc=pn,n.line=ui,n.ls=yn,n.minifyReplaces=Dl,n.onLoad=Gl,n.particle=Li,n.play=Pl,n.playBgm=zt,n.ply=nn,n.pointer=ii,n.pow=Ri,n.pr=hn,n.pw=bn,n.range=f,n.rd=cn,n.rect=xt,n.remove=I,n.rewind=Ui,n.rmv=rn,n.rnd=Ii,n.rndi=Di,n.rnds=Fi,n.round=zi,n.sin=Pi,n.sl=Mn,n.sqrt=Gi,n.stopBgm=Et,n.text=qt,n.times=C,n.tms=sn,n.tr=on,n.uc=vn,n.updateButton=kt,n.vec=xl,n.wh=an,n.wrap=B,n.yl=dn,Object.defineProperty(n,"__esModule",{value:!0})})(window||{})})();function xn(){Module.ccall("setInput","void",["boolean","boolean","boolean"],[input.isPressed,input.isJustPressed,input.isJustReleased]),Module.ccall("updateFrame","void",[],[])}let Tt=[];function Pn(){Module.ccall("initGame","void",[],[]),init({update:xn,characters:Tt,options:{isSoundEnabled:!1,isShowingScore:!1}})}window.setCharacters=(n,P)=>{let B=n;Tt=[];for(let E=0;E<P;E++){let f=`
`;for(let C=0;C<6;C++){for(let I=0;I<6;I++)f+=String.fromCharCode(Module.HEAPU8[B+I]);B+=7,f+=`
`}Tt.push(f)}};window.Module={onRuntimeInitialized:Pn};const Fl=document.createElement("script");Fl.setAttribute("src","./wasm/game.js");document.head.appendChild(Fl);
