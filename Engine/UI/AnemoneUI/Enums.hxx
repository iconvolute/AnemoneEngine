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

    enum class TextOverflowPosition
    {
        Start,
        Center,
        End,
    };

    enum class TextOverflow
    {
        Clip,
        Ellipsis,
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

    enum class Overflow
    {
        Visible,
        Hidden,
        Scroll,
    };

    enum class ResizeMode
    {
        CanMinimize,
        CanResize,
        CanResizeWithGrip,
        NoResize,
    };

    enum class VerticalAlignment
    {
        Top,
        Middle,
        Bottom,
        Stretch,
    };

    enum Docking
    {
        None,
        Left,
        Right,
        Top,
        Bottom,
        Fill,
    };

    enum class NavigationDirection
    {
        Forward,
        Backward,
    };

    enum class FormState
    {
        Normal,
        Minimized,
        Maximized,
    };

    enum class FormBorderStyle
    {
        None,
        Sizeable,
        Fixed,
        Dialog,
        FixedToolWindow,
        SizeableToolWindow,
    };

    enum class FormCaptionButton
    {
        Close,
        Help,
        Maximize,
        Minimize,
        Restore,
    };

    enum class ImageLayout
    {
        None,
        Center,
        Tile,
        Stretch,
        Zoom,
    };

    enum class Orientation
    {
        Horizontal,
        Vertical,
    };

    enum class ScrollBars
    {
        None = 0u,
        Horizontal = 1u << 0u,
        Vertical = 1u << 1u,
        Both = Horizontal | Vertical,
    };

    enum class ScrollBarVisibility
    {
        Auto,
        Visible,
        Hidden,
    };

    enum class ScrollButton
    {
        Left,
        Right,
        Top,
        Bottom,
        Min,
        Max,
    };

    enum class ScrollEventType
    {
        EndScroll,
        First,
        Last,
        PageUp,
        PageDown,
        LineUp,
        LineDown,
        ThumbPosition,
        ThumbTrack,
    };
}
