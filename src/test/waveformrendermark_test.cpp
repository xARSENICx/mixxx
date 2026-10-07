#include "test/mixxxtest.h"

#ifdef MIXXX_USE_QOPENGL
#include <QOffscreenSurface>
#include <QOpenGLContext>

#include "rendergraph/geometry.h"
#include "rendergraph/geometrynode.h"
#include "waveform/renderers/allshader/waveformrendermark.h"
#include "waveform/renderers/waveformwidgetrenderer.h"
#include "waveform/waveformwidgetfactory.h"

class WaveformRenderMarkTest : public MixxxTest {};

TEST_F(WaveformRenderMarkTest, ResizeUpdatesPlayMarkerGeometry) {
    QOffscreenSurface surface;
    surface.create();
    QOpenGLContext context;
    if (!context.create() || !context.makeCurrent(&surface)) {
        GTEST_SKIP() << "OpenGL context unavailable";
    }

    WaveformWidgetFactory::createInstance();
    {
        WaveformWidgetRenderer waveform(QString{});
        auto* pRenderer = waveform.addRenderer<allshader::WaveformRenderMark>();
        auto* pNode = static_cast<rendergraph::GeometryNode*>(pRenderer->lastChild());
        auto* pVertices =
                pNode->geometry().vertexDataAs<rendergraph::Geometry::TexturedPoint2D>();

        waveform.resizeRenderer(401, 100, 1.f);
        pRenderer->update();
        EXPECT_FLOAT_EQ(100.f, pVertices[2].position2D.y());

        waveform.resizeRenderer(401, 200, 1.f);
        pRenderer->update();
        EXPECT_FLOAT_EQ(200.f, pVertices[2].position2D.y());

        waveform.resizeRenderer(401, 50, 1.f);
        pRenderer->update();
        EXPECT_FLOAT_EQ(50.f, pVertices[2].position2D.y());

        waveform.resizeRenderer(401, 50, 2.f);
        pRenderer->update();
        EXPECT_FLOAT_EQ(195.5f, pVertices[0].position2D.x());

        waveform.setPlayMarkerPosition(0.25);
        pRenderer->update();
        EXPECT_FLOAT_EQ(95.5f, pVertices[0].position2D.x());
    }
    WaveformWidgetFactory::destroy();
}
#endif
