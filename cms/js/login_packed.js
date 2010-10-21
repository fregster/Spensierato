function jsSecureLogin(){var _1=document.forms.login.elements.username.value;var _2=document.forms.login.elements.password.value;var _3=document.forms.login.elements.security_code.value;var _4=SHA256(SHA256(_2)+_3);var _5=document.forms.login.elements.login_rememberme;if(_5.checked == true){_5=_5.value}else{_5='false'};ajaxPost(ajax_host+"/login?","username="+_1+"&key="+_4+"&login_rememberme="+_5);document.forms.searchform.elements.searchtext.value="";http.onreadystatechange=function(){if(http.readyState==4){if(http.responseText=="1"){window.location=document_root+"/"+current_page}else{alert('Login Failed, the username, password or security image was incorect. Please try again');pageReload()}}}};function SHA256(s){var _6=8;var _7=0;function safe_add(x,y){var _a=(x&65535)+(y&65535);var _b=(x>>16)+(y>>16)+(_a>>16);return(_b<<16)|(_a&65535)};function S(X,n){return(X>>>n)|(X<<(32-n))};function R(X,n){return(X>>>n)};function Ch(x,y,z){return((x&y)^((~x)&z))};function Maj(x,y,z){return((x&y)^(x&z)^(y&z))};function Sigma0256(x){return(S(x,2)^S(x,13)^S(x,22))};function Sigma1256(x){return(S(x,6)^S(x,11)^S(x,25))};function Gamma0256(x){return(S(x,7)^S(x,18)^R(x,3))};function Gamma1256(x){return(S(x,17)^S(x,19)^R(x,10))};function core_sha256(m,l){var K=new Array(1116352408,1899447441,3049323471,3921009573,961987163,1508970993,2453635748,2870763221,3624381080,310598401,607225278,1426881987,1925078388,2162078206,2614888103,3248222580,3835390401,4022224774,264347078,604807628,770255983,1249150122,1555081692,1996064986,2554220882,2821834349,2952996808,3210313671,3336571891,3584528711,113926993,338241895,666307205,773529912,1294757372,1396182291,1695183700,1986661051,2177026350,2456956037,2730485921,2820302411,3259730800,3345764771,3516065817,3600352804,4094571909,275423344,430227734,506948616,659060556,883997877,958139571,1322822218,1537002063,1747873779,1955562222,2024104815,2227730452,2361852424,2428436474,2756734187,3204031479,3329325298);var _1d=new Array(1779033703,3144134277,1013904242,2773480762,1359893119,2600822924,528734635,1541459225);var W=new Array(64);var a,b,c,d,e,f,g,h,i,j;var T1,T2;m[l>>5]|=128<<(24-l%32);m[((l+64>>9)<<4)+15]=l;for(var i=0;i<m.length;i+=16){a=_1d[0];b=_1d[1];c=_1d[2];d=_1d[3];e=_1d[4];f=_1d[5];g=_1d[6];h=_1d[7];for(var j=0;j<64;j++){if(j<16){W[j]=m[j+i]}else{W[j]=safe_add(safe_add(safe_add(Gamma1256(W[j-2]),W[j-7]),Gamma0256(W[j-15])),W[j-16])}T1=safe_add(safe_add(safe_add(safe_add(h,Sigma1256(e)),Ch(e,f,g)),K[j]),W[j]);T2=safe_add(Sigma0256(a),Maj(a,b,c));h=g;g=f;f=e;e=safe_add(d,T1);d=c;c=b;b=a;a=safe_add(T1,T2)}_1d[0]=safe_add(a,_1d[0]);_1d[1]=safe_add(b,_1d[1]);_1d[2]=safe_add(c,_1d[2]);_1d[3]=safe_add(d,_1d[3]);_1d[4]=safe_add(e,_1d[4]);_1d[5]=safe_add(f,_1d[5]);_1d[6]=safe_add(g,_1d[6]);_1d[7]=safe_add(h,_1d[7])}return _1d};function str2binb(str){var bin=Array();var _2d=(1<<_6)-1;for(var i=0;i<str.length*_6;i+=_6){bin[i>>5]|=(str.charCodeAt(i/_6)&_2d)<<(24-i%32)}return bin};function Utf8Encode(_2f){_2f=_2f.replace(/\r\n/g,"\n");var _30="";for(var n=0;n<_2f.length;n++){var c=_2f.charCodeAt(n);if(c<128){_30+=String.fromCharCode(c)}else{if((c>127)&&(c<2048)){_30+=String.fromCharCode((c>>6)|192);_30+=String.fromCharCode((c&63)|128)}else{_30+=String.fromCharCode((c>>12)|224);_30+=String.fromCharCode(((c>>6)&63)|128);_30+=String.fromCharCode((c&63)|128)}}}return _30};function binb2hex(_33){var _34=_7?"0123456789ABCDEF":"0123456789abcdef";var str="";for(var i=0;i<_33.length*4;i++){str+=_34.charAt((_33[i>>2]>>((3-i%4)*8+4))&15)+_34.charAt((_33[i>>2]>>((3-i%4)*8))&15)}return str};s=Utf8Encode(s);return binb2hex(core_sha256(str2binb(s),s.length*_6))};