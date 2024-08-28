#pragma once

namespace Anemone::UI
{
    enum class TextAlignment
    {
        Left,
        Center,
        Right,
        Justify,
    };

    enum class TextDecorationLocation
    {
        Underline,
        Overline,
        Strikethrough,
        Baseline,
    };

    enum class TextTrimming
    {
        WrapWithOverflow,
        NoWrap,
        Wrap,
    };

    enum class Visibility
    {
        Visible,
        Hidden,
        Collapsed,
    };

    enum class FlowDirection
    {
        LeftToRight,
        RightToLeft,
    };

    enum class ResizeMode
    {
        CanMinimalize,
        CanResize,
        CanResizeWithGrip,
        NoResize,
    };

    enum class HorizontalAlignment
    {
        Left,
        Center,
        Right,
        Stretch
    };

    enum class WindowStartupLocation
    {
        CenterOwner,
        CenterScreen,
        Manual,
    };

    enum class WindowState
    {
        Normal,
        Minimized,
        Maximized,
    };

    enum class WindowStyle
    {
        None,
        SingleBorder,
        Normal,
        ToolWindow,
    };

    enum class WrapDirection
    {
        None,
        Left,
        Right,
        Both,
    };

    enum class ContentAlignment
    {
        Left,
        Center,
        Right,
    };

    enum class BackgroundType
    {
        None,
        ImageFill,
        BorderFill,
    };

    enum class HitTestCode
    {
        Nowhere,
        Client,
        Left,
        Right,
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Caption,
    };

    enum class HitTestOption
    {
        Background,
        FixedBorder,
        Caption,
        ResizingBorderLeft,
        ResizingBorderTop,
        ResizingBorderRight,
        ResizingBorderBottom,
        ResizingBorder,
    };

    enum class CheckBoxState
    {
        None,

        UncheckedNormal,
        UncheckedHot,
        UncheckedPressed,
        UncheckedDisabled,
        CheckedNormal,
        CheckedHot,
        CheckedPressed,
        CheckedDisabled,
        MixedNormal,
        MixedHot,
        MixedPressed,
        MixedDisabled,
    };

    enum class ComboBoxState
    {
        None,
        Normal,
        Hot,
        Pressed,
        Disabled,
    };
}
