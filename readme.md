## rplsTOOL mod版
一言で言うとUTF-8なrplsを読み込めるようにしたrplsTOOLです.  
最近の一部のレコーダーではUTF-8でrplsを書き出す仕様があるようで,  
[4K放送番組を記録したディスクを古いディーガやプレーヤーなどで再生すると文字化けする - ブルーレイレコーダー - Panasonic](https://jpn.faq.panasonic.com/app/answers/detail/a_id/42174/~/)   
該当するrplsを読み込めるようにした物になります.
  
本家のrplsTOOL 2.10は，  
[rplsTOOL | Vesti La Giubba](http://web.archive.org/web/20180309090449/http://saysaysay.net/rplstool)  の[rplstool210.zip](http://web.archive.org/web/20180309090449/http://saysaysay.net/tools/rplstool210.zip)   
から現在入手出来ます．
## コードのライセンス等  
本家と同じく  
```
ソースコード(一部分もしくは全体)の使用に制限はありません。
何か使える機会があれば自由に使って下さい。
```  
としますので，使えるところがあったらいい感じに使ってください．  
またrplsInfoは確認してませんが，おそらく改変部分をいい感じに流用して分岐処理書けば，対応出来るようになると思います．（使わないのでやっていません）  
## ReleaseNote
2.10 mod 1(2022/05/28)  
・UTF-8なrplsの読み込みに対応  
厳密に言えばヘッダー部がPLST0300になっている物に対応．  
(※PLST0300なrpls書き出し機能, 従来のPLST0100との相互変換機能は未実装)
・放送形式の定義を追加  
高度広帯域BS/高度広帯域CS/CATVに対応．  
(もうないと思いますが，未定義分は対応するのでPullRequestください．)  
・読み込み専用時にオープンするときに読み取り専用の別プロセスに対して排他ロックを掛けないようにした．  
TSを読み込んでいるときに効果があって再生しながら開く事が出来るように．  
・ビルド環境をVisualStudio2019(のC++20準拠）に移行(に伴う修正)  

2.10以前は本家のreadmeを参照してください.