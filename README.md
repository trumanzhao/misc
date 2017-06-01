# 大杂烩

## lua/lua\_tree

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

## io_msg

简单的cpp序列化支持库,只是个玩具,编译器必须支持C++17.
由于网上已经有很多更加强大的库存在,这里就不多介绍了.

## paint
偶尔用来画图的玩具.






