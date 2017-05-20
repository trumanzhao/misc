
DashCap =
{
    DashCapFlat             = 0,
    DashCapRound            = 2,
    DashCapTriangle         = 3
};

LineCap =
{
    LineCapFlat             = 0,
    LineCapSquare           = 1,
    LineCapRound            = 2,
    LineCapTriangle         = 3,

    LineCapNoAnchor         = 0x10, -- corresponds to flat cap
    LineCapSquareAnchor     = 0x11, -- corresponds to square cap
    LineCapRoundAnchor      = 0x12, -- corresponds to round cap
    LineCapDiamondAnchor    = 0x13, -- corresponds to triangle cap
    LineCapArrowAnchor      = 0x14, -- no correspondence

    LineCapCustom           = 0xff, -- custom cap

    LineCapAnchorMask       = 0xf0  -- mask to check for anchor or not.
};

DashStyle =
{
    DashStyleSolid = 0,
    DashStyleDash = 1,
    DashStyleDot = 2,
    DashStyleDashDot = 3,
    DashStyleDashDotDot = 4,
    DashStyleCustom = 5
};

SmoothingMode =
{
    SmoothingModeDefault     = 0,
    SmoothingModeHighSpeed   = 1,
    SmoothingModeHighQuality = 2,
    SmoothingModeNone = 3,
    SmoothingModeAntiAlias = 4
};
