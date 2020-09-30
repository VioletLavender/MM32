fly  boys 11:46:06
群主，你的这个手机App用了aes对吧，手机app会把bin文件进行加密对吧

fly  boys 11:46:13
有这个操作对吧

Hush 11:46:19
手机只是透传

Hush 11:46:44
加密在keil工程里调用aes256那个dll

fly  boys 11:47:52
app没见加密呀

fly  boys 11:48:00
在哪里

Hush 11:51:49
这里面

fly  boys 11:53:02
哪里有

Hush 11:53:58
run1里面就是调用加密

Hush 11:54:09


79035684 11:54:32
配置有加密key，就会自动加密

79035684 11:54:43
没有key文件，就默认只转bin不加密

fly  boys 11:57:00
我看下

fly  boys 11:58:26
多谢大佬

fly  boys 13:55:46
大佬，这个根本不会加密的，我把这个文件放到生成hex的文件目录下面

fly  boys 13:56:10
这个不会加密呀？

Hush 13:56:14
你得自己配置啊

fly  boys 13:56:21
怎么配置

fly  boys 13:58:14
这三个要在一个文件夹对吧

Hush 13:58:23
aesKey.conf

Hush 13:58:35
输出是否加密，取决于这个文件。

Hush 13:59:21


Hush 13:59:48


fly  boys 13:59:58
这个文件你怎么打开的，我打开里面全部都是16进制

Hush 14:00:18


Hush 14:00:26
本来就是十六进制的文本啊

fly  boys 14:00:26
对的

fly  boys 14:00:40
都是意思呢

fly  boys 14:00:46
什么意思呢

Hush 14:00:58


Hush 14:01:15
对应BootLoader里的这个表

Hush 14:01:38
两边一致，就可以正常解密了。

Hush 14:02:19
aesKey.conf里，是电脑上的加密Key，用于加密，生成加密的bin

fly  boys 14:02:35
这个文件在hex to bin. exe中会自动调用对吧

Hush 14:02:42
是的

fly  boys 14:03:12
好的，多谢指点

Hush 14:03:12
kTable里面，用于解密，两边一致，烧录进去的才正确，否则，就是废文件。

fly  boys 14:03:33
多谢大佬

Hush 14:03:49
收到app通过蓝牙传进去的bin文件内容，再用kTable解密，再烧录。

fly  boys 14:04:35
多谢，多谢了

fly  boys 14:21:37
对，确实是会加密的，我对比较两个bin文件

fly  boys 14:22:21
我直接copy你的这三个文件但是就是不行，你的配置我不能直接用吗？

Hush 14:22:49
keil工程里也要配置

fly  boys 14:23:08
我也是copy你的配置

fly  boys 14:23:23
能够生成正确的bin文件但是不会加密

Hush 14:24:00
那就肯定还是哪儿没配对呗

fly  boys 14:24:11


Hush 14:29:27
你这个能加密才怪了

fly  boys 14:30:19
???

fly  boys 14:30:31
哪里的问题？

Hush 14:32:56
你再仔细看看

fly  boys 14:34:29
没感觉哪里有问题呀

Hush 14:35:19
你用的fromelf

Hush 14:35:26
压根儿了没用我的工具

fly  boys 14:36:01
你的也是这样的呀？

Hush 14:36:37
你确定？

fly  boys 14:36:59
你看下

fly  boys 14:37:27
我错了

Hush 14:38:03
我不用看都知道你弄错了


