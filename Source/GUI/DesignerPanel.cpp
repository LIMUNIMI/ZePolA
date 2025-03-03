#include "DesignerPanel.h"
#include "../DSP/FilterDesign.h"

// =============================================================================
DesignerPanel::DesignerPanel(VTSAudioProcessor& p) : processor(p)
{
    // !REMOVE THIS! Test FilterDesign
    FilterParameters params(processor.getSampleRate());
    params.cutoff = 500.0;
    params.order  = 4;
    params.computeZPK();

    auto degree = params.zpk.degree();
    auto k_db   = juce::Decibels::gainToDecibels(params.zpk.gain, -1000.0)
                / (params.zpk.zeros.size() + params.zpk.poles.size());
    for (auto i = 0; i < degree; ++i)
    {
        if (i < params.zpk.zeros.size())
            DBG("  Z(" << abs(params.zpk.zeros[i]) << "; "
                       << std::arg(params.zpk.zeros[i])
                              / juce::MathConstants<double>::pi
                       << ") " << k_db << "dB");
        if (i < params.zpk.poles.size())
            DBG("  P(" << abs(params.zpk.poles[i]) << "; "
                       << std::arg(params.zpk.poles[i])
                              / juce::MathConstants<double>::pi
                       << ") " << k_db << "dB");
    }
    // !REMOVE THIS!
}
