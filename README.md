# 大杂烩

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
