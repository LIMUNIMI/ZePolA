#pragma once
#include <JuceHeader.h>
#include "Parameters.h"
#include <vector>
#include <memory>

// La classe FilterElement rappresenta un numero complesso in modulo-fase. Un ZERO è un valore del piano complesso per il quale la funzione di trasferimento del filtro digitale diventa zero. Un POLE è un valore del piano complesso per il quale la funzione di trasferimento del filtro digitale diventa infinita. In generale la funzione di trasferimento di un filtro IIR è rappresentato come una polinomiale fratta: gli zeri sono le radici del numeratore (parte FIR del filtro) mentre i poli sono le radici del denominatore (parte IIR del filtro).
// L'approccio per la creazione della classe FilterElement è quello di scomporre la catena di poli e zeri di un filtro digitale di ordine M in singoli filtri da uno zero o da un polo in cascata.
// Ciascun FilterElement è composto dai seguenti parametri: magnitudine (modulo), fase, coeff1, coeff2 per i coefficienti dell'equazione alle differenze e per il calcolo del sample in uscita e da due variabili double per la memoria del filtro: sample precedente e due sample fa (in entrata se si tratta di uno ZERO e in uscita se si tratta di un POLE).
class FilterElement
{
public:
    
    enum Type { ZERO, POLE };
    
    // Il costruttore di FilterElement richiede obbligatoriamente la specifica del tipo dell'elemento (ZERO o POLO) mentre per magnitudine e fase sono descritti i valori di default: 0.0 per entrambi.
    FilterElement (Type t, double mg = 0.0, double ph = 0.0) : type(t), magnitude(mg), phase(ph)
    {
        calculateCoefficients();
    }
    
    ~FilterElement () {}
    
    // Il metodo setMagnitude imposta la magnitudine del numero complesso con il valore di newValue e chiama il metodo calculateCoefficients per il calcolo dei nuovi coefficienti per l'equazione alle differenze.
    void setMagnitude (double newValue)
    {
        magnitude = newValue;
        calculateCoefficients();
    }
    
    // Il metodo setPhase imposta la fase del numero complesso con il valore di newValue e chiama il metodo calculateCoefficients per il calcolo dei nuovi coefficienti per l'equazione alle differenze.
    void setPhase (double newValue)
    {
        phase = newValue;
        calculateCoefficients();
    }
    
    // Il metodo getMagnitude restituisce la magnitudine (modulo) del numero complesso rappresentato da FilterElement.
    double getMagnitude ()
    {
        return magnitude;
    }
    
    // Il metodo getPhase restituisce la fase del numero complesso rappresentato da FilterElement.
    double getPhase ()
    {
        return phase;
    }
    
    // Il metodo getRealPart restituisce la parte reale del numero complesso rappresentato da FilterElement.
    double getRealPart ()
    {
        return getMagnitude() * cos(getPhase() * MathConstants<double>::pi);
    }

    // Il metodo getType restituisce il tipo del FilterElement.
    enum Type getType ()
    {
        return type;
    }
    
    // Il metodo memoryReset riporta a zero la memoria del FilterElement (sia esso uno ZERO o un POLO).
    void memoryReset ()
    {
        memory1 = 0.0;
        memory2 = 0.0;
    }
    
    // Il metodo calculateCoefficients calcola, per il FilterElement, i coefficienti da utilizzare nel calcolo del sample in uscita. Per mantenere la simmetria hermitiana del filtro ogni zero e ogni polo hanno il loro coniugato. Il che significa che il calcolo dei coefficienti, sia nel caso di polo, che nel caso di zero si riduce a: somma dei coniugati (due volte la parte reale) e prodotto tra coniugati (quadrato del modulo).
    void calculateCoefficients ()
    {
        coeff1 = -2 * getRealPart();
        coeff2 = magnitude * magnitude;
    }
    
    // Il metodo processSample viene chiamato per ogni sample del buffer in ingresso e processa un campione in entrata (istante attuale) calcolando il campione in uscita. È differente nel caso di elemento ZERO o elemento POLO. L'elemento ZERO calcola solamente la parte FIR (non ricorsiva) dell'equazione alle differenze: il sample in uscita dall'elemento ZERO è calcolata come somma tra il sample in entrata, il sample in entrata di un istante fa (moltiplicato per il coefficiente coeff1) e il sample in entrata due istanti fa (moltiplicato per il coefficiente coeff2). Con istante fa e due istanti fa si intende sample precedente e due sample fa. L'elemento POLO calcola solamente la parte IIR (ricorsiva) dell'equazione alle differenze: il sample in uscita dall'elemento POLO è calcolata come differenza tra il sample in entrata, il sample in uscita un istante fa (moltiplicato per il coefficiente coeff1) e il sample in uscita due istanti fa (moltiplicato per il coefficiente coeff2). Viene successivamente aggiornata la memoria del FilterElement per avere a disposizione la memoria per il calcolo del sample successivo nella chiamata al metodo processSample successivo.
    float processSample (double inputSample)
    {
        double outputSample;
        switch (type)
        {
            case ZERO:
            {
                outputSample = inputSample + coeff1 * memory1 + coeff2 * memory2;
            } break;
                
            case POLE:
            {
                outputSample = inputSample - coeff1 * memory1 - coeff2 * memory2;
            } break;
        }
        
        updateMemory(inputSample, outputSample);
        return outputSample;
    }
    
    // Il metodo processBlock riceve in input l'intero buffer di campioni audio e chiama la processSample per ciascuno dei campioni del buffer.
    void processBlock (juce::AudioBuffer<double>& buffer, int numSamples)
    {
        auto bufferData = buffer.getArrayOfWritePointers();
        
        for (int smp = 0; smp < numSamples; ++smp)
                bufferData[0][smp] = processSample(bufferData[0][smp]);
    }
    
    // Il metodo updateMemory aggiorna la memoria del filtro. memory1 contiene il sample precedente in entrata se si tratta di uno ZERO o in uscita se si tratta di un POLE. memory2 contiene il sample di due istanti fa in entrata se si tratta di uno ZERO o in uscita se si tratta di un POLE.
    void updateMemory (double inputSample, double outputSample)
    {
        memory2 = memory1;
        switch (type)
        {
            case ZERO:
            {
                memory1 = inputSample;
            } break;
                
            case POLE:
            {
                memory1 = outputSample;
            }
        }
    }
    
private:
    Type type;
    
    double magnitude;
    double phase;
    
    double coeff1;
    double coeff2;

    double memory1;
    double memory2;
};


class PolesAndZerosCascade
{
public:
    PolesAndZerosCascade (int nZeros = 1, int nPoles = 1)
    {
        for (int i = 0; i < nZeros; ++ i)
            addElement(FilterElement::ZERO);
        
        for (int i = 0; i < nPoles; ++ i)
            addElement(FilterElement::POLE);
    }
    
    ~PolesAndZerosCascade () {}
    
    void memoryReset ()
    {
        for (auto& node : elements)
            node->memoryReset();
    }
    
    void setUnsetBypass (bool newValue)
    {
        active = !newValue;
        if (!active)
            memoryReset();
    }
    
    void addElement (FilterElement::Type type)
    {
        if (countElementsOfType(type) < MAX_ORDER)
            elements.push_back(std::make_unique<FilterElement>(type));
        else
        {
            DBG(" You are trying to add a zero or a pole even if you have reached the maximum number of poles or zeros!");
            jassertfalse; // You are trying to add a zero or a pole even if you have reached the maximum number of poles or zeros!
        }
    }
    
    void removeElement (FilterElement::Type type)
    {
        auto iterator = std::find_if(elements.rbegin(), elements.rend(),
                                   [type](const std::unique_ptr<FilterElement>& element) { return element->getType() == type; });
        // find_if ritorna un iteratore all'ultimo elemento che corrisponde al criterio di ricerca (type da eliminare)
        if (iterator != elements.rend())
            elements.erase(std::next(iterator).base());
        else
        {
            DBG("No element of the specified type found!");
            jassertfalse;
        }
    }
    
    template <typename TargetType, typename SourceType>
    void castBuffer(AudioBuffer<TargetType>& destination, const AudioBuffer<SourceType>& source, const int numChannels, const int numSamples)
    {
        auto dst = destination.getArrayOfWritePointers();
        auto src = source.getArrayOfReadPointers();

        for (int ch = 0; ch < numChannels; ++ch)
            for (int smp = 0; smp < numSamples; ++smp)
                dst[ch][smp] = static_cast<TargetType>(src[ch][smp]);
    }
    
    void processBlock (juce::AudioBuffer<float>& buffer)
    {
        if (!active) return;
        
        const auto numSamples = buffer.getNumSamples(); // Calcolo del numero dei campioni su cui il filtro lavora, per non dover chiamare il metodo più volte si preferisce assegnare una variabile passata per copia a tutti i FilterElement
        
        double const referenceRMS = buffer.getRMSLevel(0, 0, numSamples); // Calcolo RMS del buffer dry prima del filtro
        
        AudioBuffer<double> doubleBuffer(1, numSamples); // Creazione del buffer double che utilizzerà il filtro
        
        castBuffer(doubleBuffer, buffer, 1, numSamples); // Cast del buffer<float> in buffer<double>
        
        for (auto& element : elements) // Loop che chiama la processBlock di ciascuno degli elementi (FilterElement) attivi
            element->processBlock(doubleBuffer, numSamples);
            
        castBuffer(buffer, doubleBuffer, 1, numSamples); // Cast del buffer<double> in buffer<float>
    
        double const gain = referenceRMS / buffer.getRMSLevel(0, 0, numSamples); // Calcolo del gain da applicare al buffer wet in uscita dal filtro mediante il rapporto tra il referenceRMS e il valore RMS del buffer wet
        
        buffer.applyGain(0, 0, numSamples, static_cast<float>(gain)); // Applicazione del gain al buffer in uscita dal filtro
    }
    
    void parameterChanged (const String& parameterID, float newValue)
    {
        if (parameterID == "BYPASS")
        {
            setUnsetBypass(newValue > 0.5f);
            return;
        }
        
        juce::String type = parameterID.dropLastCharacters(2);
        FilterElement::Type elementType = (type == "Z") ? FilterElement::ZERO : FilterElement::POLE;
        juce::String elementParameter = parameterID.substring(1, 2);
        const int elementNr = parameterID.getLastCharacters(1).getIntValue();

        int i = 0;
        
        for (auto& element : elements)
        {
            if (element->getType() == elementType)
            {
                ++ i;
                if (i == elementNr)
                    (elementParameter == "M") ? element->setMagnitude(newValue) : element->setPhase(newValue);
            }
        }
    }
    
private:
    std::vector<std::unique_ptr<FilterElement>> elements;
    
    bool active = true;
    
    int countElementsOfType(FilterElement::Type type)
    {
        return static_cast<int>(std::count_if(elements.begin(), elements.end(),
                                     [type](const std::unique_ptr<FilterElement>& element) { return element->getType() == type; }));
    }
};


