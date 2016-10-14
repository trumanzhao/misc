# 大杂烩

## luna
一个lua的C++绑定库,参见luna.md.
原本放在luna库中的,现在luna库挪着他用了.
现在的luna库用着一个完整服务端框架的实现.
里面虽然也有luna.cpp,luna.h,但是跟这里的版本有所不同.
关键的不同在于对C++导出对象的生命期管理.
在这里的luna.cpp/luna.h中,导出对象是以C++代码来控制生命期的.
而新的luna库中的版本,则是以lua的gc来维护的,并把文件沙盒相关的代码也实现到了lua中.

## lua\_tree

文件: tree.lua
函数: log\_tree(desc, var);
用途: 在lua中树状打印table.

``` lua
local player =
{
    name = "bitch",
    level = 10,
    id = 123,
    items =
    {
        [1] = "some_item_1",
        [2] = "some_item_2"
    }
};
log_tree("player_data", player);
```

输出文本:

```
player_data
├─ items
│  ├─ 2: "some_item_2"
│  └─ 1: "some_item_1"
├─ level: 10
├─ name: "bitch"
└─ id: 123
```
