// 4色の猫の画像を再生成する。追加パッケージは不要。
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
// 引数で再生成先のスケッチフォルダを指定できる。標準はMultiView。
const dir=process.argv[2] ? path.resolve(process.argv[2]) :
  path.resolve(path.dirname(fileURLToPath(import.meta.url)),'../firmware/LevelSketch/examples/MultiView');
function raster(w,h) {
  const pixels=new Uint8Array(w*h);
  function put(x,y,v){x=Math.round(x);y=Math.round(y);if(x>=0&&x<w&&y>=0&&y<h)pixels[y*w+x]=v;}
  function ellipse(cx,cy,rx,ry,v){for(let y=0;y<h;y++)for(let x=0;x<w;x++)if(((x-cx)/rx)**2+((y-cy)/ry)**2<=1)put(x,y,v);}
  function line(points,width,v){for(let i=1;i<points.length;i++){const [x0,y0]=points[i-1],[x1,y1]=points[i],n=Math.max(1,Math.ceil(Math.hypot(x1-x0,y1-y0)*3));for(let j=0;j<=n;j++)ellipse(x0+(x1-x0)*j/n,y0+(y1-y0)*j/n,width/2,width/2,v);}}
  function polygon(points,v){for(let y=0;y<h;y++)for(let x=0;x<w;x++){let inside=false;for(let i=0,j=points.length-1;i<points.length;j=i++){const [a,b]=points[i],[c,d]=points[j];if((b>y)!==(d>y)&&x<(c-a)*(y-b)/(d-b)+a)inside=!inside;}if(inside)put(x,y,v);}}
  const pack=()=>{const bytes=[];for(let i=0;i<pixels.length;i+=4)bytes.push((pixels[i]<<6)|(pixels[i+1]<<4)|(pixels[i+2]<<2)|pixels[i+3]);return bytes;};
  return {w,h,pixels,put,ellipse,line,polygon,pack};
}
// 演奏画面の顔は待機用の絵と個別に定義する。
function meterHead(r,cx,cy,scale,closed=false) {
  const shape=[[-14,-2],[-12,-7],[-12,-15],[-5,-8],[5,-8],[12,-15],[12,-7],[14,-2],[13,5],[8,9],[-8,9],[-13,5]];
  const points=shape.map(([x,y])=>[cx+x*scale,cy+y*scale]);
  r.polygon(points,1);r.line([...points,points[0]],1,2);
  // 黒目が消えないよう、画素の整数位置にそろえる。
  for(const side of [-1,1]) {
    const ex=Math.round(cx+side*6.5*scale),ey=Math.round(cy);
    if(closed) r.line([[ex-2*scale,ey],[ex+2*scale,ey]],1,3);
    else {
      r.ellipse(ex,ey,Math.max(2,2.5*scale),Math.max(1,2*scale),3);
      const halfHeight=scale<1?0:1;
      for(let y=ey-halfHeight;y<=ey+halfHeight;y++) r.put(ex,y,1);
    }
  }
  r.ellipse(cx,cy+5*scale,.9,.9,2);
}
function patrolHead(r,cx,cy,view,closed=false) {
  const front=view==='front',quarter=view==='quarter',mask=raster(r.w,r.h);
  const poly=pts=>pts.map(([x,y])=>[cx+x,cy+y]);
  if(front) {
    mask.polygon(poly([[-8,-3],[-8,-9],[-3,-5]]),1);
    mask.polygon(poly([[8,-3],[8,-9],[3,-5]]),1);
    mask.ellipse(cx,cy+1,9,7.5,1);
  } else {
    mask.polygon(poly([[-6,-2],[-7,-9],[-2,-4]]),1);
    mask.polygon(poly([[-3,-3],[-3,-11],[4,-3]]),1);
    mask.ellipse(cx-1,cy+1,quarter?8.5:7.8,7.5,1);
    mask.ellipse(cx+(quarter?4:5),cy+3,3.4,3.2,1);
  }
  // 頬から口元までひと続きに輪郭を描く。
  for(let y=1;y<r.h-1;y++)for(let x=1;x<r.w-1;x++) {
    const i=y*r.w+x;if(!mask.pixels[i])continue;
    r.put(x,y,(!mask.pixels[i-1]||!mask.pixels[i+1]||!mask.pixels[i-r.w]||!mask.pixels[i+r.w])?2:1);
  }
  const ey=Math.round(cy),eyes=front?[-4,4]:quarter?[-3,4]:[3];
  eyes.forEach((offset,index)=>{
    const ex=Math.round(cx+offset),far=quarter&&index===0;
    if(closed) r.line([[ex-2,ey+1],[ex,ey],[ex+2,ey+1]],1.2,3);
    else if(far){r.put(ex,ey,3);r.put(ex,ey+1,3);}
    else {
      r.ellipse(ex,ey+1,2.5,2,3);
      for(let y=ey-1;y<=ey;y++)for(let x=ex-3;x<=ex+3;x++)r.put(x,y,1);
      r.put(ex,ey+1,1);r.put(ex,ey+2,1);
    }
  });
  const nx=Math.round(cx+(front?0:quarter?6:8)),ny=Math.round(cy+3);
  r.put(nx,ny,2);
  if(front){r.put(nx-1,ny+2,2);r.put(nx+1,ny+2,2);}
  else{r.put(nx-3,ny+1,2);r.put(nx-2,ny+1,2);}
}
const walk=[];
// 0〜3は横向きの歩行、4は正面、5は目を閉じた伸び、6は斜め向き。
for(let frame=0;frame<7;frame++){
  const r=raster(48,32),scale=.72;
  const p=points=>points.map(([x,y])=>[24+x*scale,31+y*scale]);
  const fur=(points,width=4)=>{r.line(p(points),(width+1.5)*scale,2);r.line(p(points),width*scale,1);};
  const step=[-4,0,4,0,0,0,0][frame],tail=[-2,0,2,0,0,0,0][frame];
  if(frame===5){
    fur([[-20,-19],[-29,-25],[-28,-34]]);fur([[-17,-22],[-18,-2],[-11,-1]],5);
    fur([[-15,-22],[7,-13],[14,-6]],14);r.line(p([[10,-10],[23,-1],[30,-1]]),4*scale,1);
    patrolHead(r,24+19*scale,31-12*scale,'profile',true);
  }else{
    fur([[-16,-17],[-26,-20],[-28,-29+tail],[-24,-33+tail]],5);
    fur([[-10,-12],[-8-step,-3],[-3-step,-1]],5);fur([[8,-12],[10+step,-3],[15+step,-1]],5);
    r.ellipse(24-3*scale,31-17*scale,18*scale,12*scale,2);r.ellipse(24-3*scale,31-17*scale,17*scale,11*scale,1);
    r.line(p([[-16,-12],[-17+step,-3],[-12+step,-1]]),5*scale,1);r.line(p([[6,-12],[6-step,-3],[11-step,-1]]),5*scale,1);
    patrolHead(r,24+17*scale,31-26*scale,frame===4?'front':frame===6?'quarter':'profile');
  }
  walk.push(r.pack());
}
const faces=[false,true].map(closed=>{const r=raster(32,28);meterHead(r,15,17,1,closed);return r.pack();});
function array(name,frames){return `const uint8_t ${name}[${frames.length}][${frames[0].length}] PROGMEM = {\n`+frames.map(bytes=>'  {\n'+Array.from({length:Math.ceil(bytes.length/16)},(_,i)=>'    '+bytes.slice(i*16,i*16+16).map(v=>'0x'+v.toString(16).padStart(2,'0')).join(', ')+',').join('\n')+'\n  },').join('\n')+'\n};\n';}
const rgb565=(r,g,b)=>((r>>3)<<11)|((g>>2)<<5)|(b>>3);
const palette=[0,rgb565(52,60,83),rgb565(153,158,195),rgb565(255,208,112)];
// スケッチ側のヘッダーで宣言された画像だけを書き出す。
const header = fs.readFileSync(path.join(dir, 'BlackCatArt.h'), 'utf8');
const artwork = [
  ['BLACK_CAT_WALK', walk],
  ['BLACK_CAT_FACE', faces],
].filter(([name]) => header.includes(name))
 .map(([name, frames]) => array(name, frames)).join('');
if (!artwork) throw new Error('BlackCatArt.h に猫の画像の宣言がありません。');
fs.writeFileSync(path.join(dir,'BlackCatArt.cpp'),'// tools/generate_cat_art.mjs で生成。色番号0は透明。\n#include "BlackCatArt.h"\nnamespace SketchViews {\nconst uint16_t BLACK_CAT_PALETTE[4] PROGMEM = {'+palette.map(v=>'0x'+v.toString(16)).join(', ')+'};\n'+artwork+'}\n');
