#pragma once
#include <JuceHeader.h>
#include "Parameters.h"
#include <vector>
#include <memory>

// ------------------------------------------------------------------------------------------------------------------------------------
// ITA
// La classe "FilterElement" rappresenta un numero complesso in modulo-fase. Un ZERO è un valore del piano complesso per il quale la funzione di trasferimento del filtro digitale diventa zero. Un POLE è un valore del piano complesso per il quale la funzione di trasferimento del filtro digitale diventa infinita. In generale la funzione di trasferimento di un filtro IIR è rappresentato come una polinomiale fratta: gli zeri sono le radici del numeratore (parte FIR del filtro) mentre i poli sono le radici del denominatore (parte IIR del filtro).
// L'approccio per la creazione della classe FilterElement è quello di scomporre la catena di poli e zeri di un filtro digitale di ordine M in singoli filtri da uno zero o da un polo in cascata e di rappresentare il più piccolo componente di un filtro digitale.
// Ciascun FilterElement è composto dai seguenti parametri: magnitudine (modulo), fase, coeff1, coeff2 per i coefficienti dell'equazione alle differenze e per il calcolo del sample in uscita e da due variabili double per la memoria del filtro: sample precedente e due sample fa (in entrata se si tratta di uno ZERO e in uscita se si tratta di un POLE).

// ENG
// The class "FilterElement" represents a complex number in magnitude-phase form. A ZERO is a value on the complex plane for which the transfer function of a digital filter becomes zero. A POLE is a value on the complex plane for which the transfer function of a digital filter becomes infinite. In general, the transfer function of an IIR filter is represented as a rational polynomial: the zeros are the roots of the numerator (FIR part of the filter) while the poles are the roots of the denominator (IIR part of the filter).
// The approach for creating the FilterElement class is to break down the chain of poles and zeros of a digital filter of order M into individual filters with either a single zero or a single pole in cascade and to represent the smallest component of a digital filter.
// Each FilterElement consists of the following parameters: magnitude (modulus), phase, coeff1, and coeff2 for the coefficients of the difference equation and for calculating the output sample, along with two double variables for the filter memory: the previous sample and the sample from two instances ago (input samples if it is a ZERO and output samples if it is a POLE).

class FilterElement
{
public:
    
    enum Type { ZERO, POLE };
    
    // ITA - Il costruttore di FilterElement richiede obbligatoriamente la specifica del tipo dell'elemento (ZERO o POLO) mentre per magnitudine e fase sono descritti i valori di default: 0.0 per entrambi.
    // ENG - The constructor of FilterElement requires the specification of the element type (ZERO or POLE), while magnitude and phase have default values: 0.0 for both.
    FilterElement (Type t, double mg = 0.0, double ph = 0.0) : type(t), magnitude(mg), phase(ph)
    {
        calculateCoefficients();
    }
    
    ~FilterElement () {}
    
    // ITA - Il metodo setMagnitude imposta la magnitudine del numero complesso con il valore di newValue e chiama il metodo calculateCoefficients per il calcolo dei nuovi coefficienti per l'equazione alle differenze.
    // ENG - The method setMagnitude sets the magnitude of the complex number to the value of newValue and calls the calculateCoefficients method to compute the new coefficients for the difference equation.
    void setMagnitude (double newValue)
    {
        magnitude = newValue;
        calculateCoefficients();
    }
    
    // ITA - Il metodo setPhase imposta la fase del numero complesso con il valore di newValue e chiama il metodo calculateCoefficients per il calcolo dei nuovi coefficienti per l'equazione alle differenze.
    // ENG - The method setPhase sets the phase of the complex number to the value of newValue and calls the calculateCoefficients method to compute the new coefficients for the difference equation.
    void setPhase (double newValue)
    {
        phase = newValue;
        calculateCoefficients();
    }
    
    // ITA - Il metodo getMagnitude restituisce la magnitudine (modulo) del numero complesso rappresentato da FilterElement.
    // ENG - The method getMagnitude returns the magnitude of the complex number represented by FilterElement.
    double getMagnitude ()
    {
        return magnitude;
    }
    
    // ITA - Il metodo getPhase restituisce la fase del numero complesso rappresentato da FilterElement.
    // ENG - The method getPhase returns the phase of the complex number represented by FilterElement.
    double getPhase ()
    {
        return phase;
    }
    
    // ITA - Il metodo getRealPart restituisce la parte reale del numero complesso rappresentato da FilterElement.
    // ENG - The method getRealPart returns the real part of the complex number represented by FilterElement.
    double getRealPart ()
    {
        return getMagnitude() * cos(getPhase() * MathConstants<double>::pi);
    }

    // ITA - Il metodo getType restituisce il tipo del FilterElement.
    // ENG - The method getType returns the type of the FilterElement.
    enum Type getType ()
    {
        return type;
    }
    
    // ITA - Il metodo memoryReset riporta a zero la memoria del FilterElement (sia esso uno ZERO o un POLO).
    // ENG - The memoryReset method resets the memory of the FilterElement (whether it is a ZERO or a POLE) to zero.
    void memoryReset ()
    {
        memory1 = 0.0;
        memory2 = 0.0;
    }
    
    // ITA - Il metodo calculateCoefficients calcola, per il FilterElement, i coefficienti da utilizzare nel calcolo del sample in uscita. Per mantenere la simmetria hermitiana del filtro ogni zero e ogni polo hanno il loro coniugato. Il che significa che il calcolo dei coefficienti, sia nel caso di polo, che nel caso di zero si riduce alla somma dei coniugati (due volte la parte reale) e al prodotto tra coniugati (quadrato del modulo).
    // ENG - The method calculateCoefficients calculates the coefficients to be used by the FilterElement in computing the output sample. To maintain the Hermitian symmetry of the filter, each zero and each pole have their conjugate. This means that the coefficient calculation, whether for a pole or a zero, reduces to the sum of the conjugates (twice the real part) and the product of the conjugates (the square of the magnitude).
    void calculateCoefficients ()
    {
        coeff1 = -2 * getRealPart();
        coeff2 = magnitude * magnitude;
    }
    
    // ITA - Il metodo processSample viene chiamato per ogni sample del buffer in ingresso e processa un campione in entrata (istante attuale) calcolando il campione in uscita. È differente nel caso di elemento ZERO o elemento POLO. L'elemento ZERO calcola solamente la parte FIR (non ricorsiva) dell'equazione alle differenze: il sample in uscita dall'elemento ZERO è calcolata come somma tra il sample in entrata, il sample in entrata di un istante fa (moltiplicato per il coefficiente coeff1) e il sample in entrata due istanti fa (moltiplicato per il coefficiente coeff2). Con istante fa e due istanti fa si intende sample precedente e due sample fa. L'elemento POLO calcola solamente la parte IIR (ricorsiva) dell'equazione alle differenze: il sample in uscita dall'elemento POLO è calcolata come differenza tra il sample in entrata, il sample in uscita un istante fa (moltiplicato per il coefficiente coeff1) e il sample in uscita due istanti fa (moltiplicato per il coefficiente coeff2). Viene successivamente aggiornata la memoria del FilterElement per avere a disposizione la memoria per il calcolo del sample successivo nella chiamata al metodo processSample successivo.
    // ENG - The processSample method is called for each sample in the incoming buffer and processes an input sample (the current instant) by calculating the output sample. The processing differs depending on whether it is a ZERO element or a POLE element. The ZERO element calculates only the FIR (non-recursive) part of the difference equation: the output sample from the ZERO element is calculated as the sum of the input sample, the input sample from one instant ago (multiplied by the coefficient coeff1), and the input sample from two instants ago (multiplied by the coefficient coeff2). One instant ago and two instants ago refer to the previous sample and the sample before that, respectively. The POLE element calculates only the IIR (recursive) part of the difference equation: the output sample from the POLE element is calculated as the difference between the input sample, the output sample from one instant ago (multiplied by the coefficient coeff1), and the output sample from two instants ago (multiplied by the coefficient coeff2). The memory of the FilterElement is then updated to have the necessary information available for the calculation of the next sample during the subsequent call to the processSample method.
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
    
    // ITA - Il metodo processBlock riceve in input l'intero buffer di campioni audio e chiama la processSample per ciascuno dei campioni del buffer.
    // ENG - The processBlock method takes the entire buffer of audio samples as input and calls the processSample method for each sample in the buffer.
    void processBlock (juce::AudioBuffer<double>& buffer, int numSamples)
    {
        auto bufferData = buffer.getArrayOfWritePointers();
        
        for (int smp = 0; smp < numSamples; ++smp)
                bufferData[0][smp] = processSample(bufferData[0][smp]);
    }
    
    // ITA - Il metodo updateMemory aggiorna la memoria del filtro. memory1 contiene il sample precedente in entrata se si tratta di uno ZERO o in uscita se si tratta di un POLE. memory2 contiene il sample di due istanti fa in entrata se si tratta di uno ZERO o in uscita se si tratta di un POLE.
    // ENG - The updateMemory method updates the filter's memory. memory1 stores the previous sample, which is an input sample if it is a ZERO or an output sample if it is a POLE. memory2 stores the sample from two instances ago, which is an input sample if it is a ZERO or an output sample if it is a POLE.
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

// ------------------------------------------------------------------------------------------------------------------------------------
// ITA
// La classe "PolesAndZerosCascade" rappresenta filtro digitale come catena di poli e zeri (filtri semplici). L'approccio della classe è quello di concepire un filtro digitale IIR come catena di filtri digitali semplici ciascuno composto da un solo zero (filtro FIR) oppure da un solo polo (filtro IIR).
// Un PolesAndZerosCascade è composto dai seguenti parametri: un std::vector di puntatori a oggetti di tipo FilterElement (poli o zeri) in modo da poter allocare dinamicamente i componenti del filtro e decidere arbitrariamente quanti poli e quanti zeri associare al filtro, un booleano per gestire il bypass del filtro. La lista puntata di FilterElement segue la logica LIFO (last in - first out). L'elemento più recentemente aggiunto al filtro è il primo a essere eliminato in seguito alla richiesta dell'utente.
// Il metodo privato countElementsOfType conta gli elementi del tipo specificato nella chiamata (POLE o ZERO).

// ENG
// The "PolesAndZerosCascade" class represents a digital filter as a chain of poles and zeros (simple filters). The approach of the class is to conceive an IIR digital filter as a chain of simple digital filters, each composed of either a single zero (FIR filter) or a single pole (IIR filter).
// A PolesAndZerosCascade is composed of the following parameters: a std::vector of pointers to objects of type FilterElement (poles or zeros) to dynamically allocate the filter components and arbitrarily decide how many poles and how many zeros to associate with the filter, and a boolean to manage the filter's bypass. The list of FilterElement pointers follows a LIFO (last in - first out) logic. The most recently added element to the filter is the first to be removed upon the user's request.
// The private method countElementsOfType counts the elements of the specified type (POLE or ZERO) in the call.
class PolesAndZerosCascade
{
public:
    
    // ITA - Il costruttore di PolesAndZerosCascade ha come valori di default il numero di zeri e di poli che deve avere il filtro al momento della creazione.
    // ENG - The constructor of PolesAndZerosCascade has default values for the number of zeros and poles that the filter should have at the time of creation.
    PolesAndZerosCascade (int nZeros = 1, int nPoles = 1)
    {
        for (int i = 0; i < nZeros; ++ i)
            addElement(FilterElement::ZERO);
        
        for (int i = 0; i < nPoles; ++ i)
            addElement(FilterElement::POLE);
    }
    
    ~PolesAndZerosCascade () {}
    
    // ITA - Il metodo memoryReset resetta la memoria dell'intero filtro, chiamando il metodo di reset per tutti gli elementi che componongo la cascata del filtro stesso.
    // ENG - The memoryReset method resets the memory of the entire filter by calling the reset method for all the elements that make up the filter's cascade.
    void memoryReset ()
    {
        for (auto& node : elements)
            node->memoryReset();
    }
    
    // ITA - Il metodo setUnsetBypass bypassa il filtro per intero a seconda del valore in newValue scelto dall'utente come parametro del plugin.
    // ENG - The setUnsetBypass method bypasses the entire filter depending on the value of newValue chosen by the user as a parameter of the plugin.
    void setUnsetBypass (bool newValue)
    {
        active = !newValue;
        if (!active)
            memoryReset();
    }
    
    // ITA - Il metodo addElement aggiunge un elemento del tipo specificato alla cascata di elementi zero o polo. L'aggiunta avviene in fondo alla lista puntata di elementi.
    // ENG - The addElement method adds an element of the specified type to the cascade of zero or pole elements. The addition occurs at the end of the list of pointed elements.
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
    
    // ITA - Il metodo removeElement rimuove un elemento del tipo specificato alla cascata di elementi zero o polo. Viene eliminato l'elemento inserito più recentemente che corrisponde al tipo richiesto.
    // ENG - The removeElement method removes an element of the specified type from the cascade of zero or pole elements. The most recently added element that matches the specified type is the one that gets removed.
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
    
    // ITA - Il metodo castBuffer nella classe PolesAndZerosCascade è una funzione template che converte e copia dati audio tra due buffer di tipi di dati potenzialmente diversi. In input sono presenti quattro parametri: un riferimento al buffer audio di destinazione AudioBuffer<TargetType>& destination, un riferimento al buffer audio sorgente const AudioBuffer<SourceType>& source, il numero di canali da processare const int numChannels e il numero di campioni per canale const int numSamples. Il metodo funziona ottenendo innanzitutto i puntatori alla memoria scrivibile nel buffer di destinazione e alla memoria leggibile nel buffer sorgente. Successivamente, itera su ciascun canale e ciascun campione, convertendo i dati dal tipo sorgente SourceType al tipo di destinazione TargetType utilizzando un cast e memorizzando il risultato nel buffer di destinazione.
    // ENG - The castBuffer method in the PolesAndZerosCascade class is a template function that converts and copies audio data between two buffers of potentially different data types. It takes four input parameters: a reference to the destination audio buffer AudioBuffer<TargetType>& destination, a reference to the source audio buffer const AudioBuffer<SourceType>& source, the number of channels to process const int numChannels, and the number of samples per channel const int numSamples. The method works by first obtaining pointers to the writable memory in the destination buffer and the readable memory in the source buffer. It then iterates over each channel and each sample, converting the data from the source type SourceType to the destination type TargetType using a cast, and storing the result in the destination buffer.
    template <typename TargetType, typename SourceType>
    void castBuffer(AudioBuffer<TargetType>& destination, const AudioBuffer<SourceType>& source, const int numChannels, const int numSamples)
    {
        auto dst = destination.getArrayOfWritePointers();
        auto src = source.getArrayOfReadPointers();

        for (int ch = 0; ch < numChannels; ++ch)
            for (int smp = 0; smp < numSamples; ++smp)
                dst[ch][smp] = static_cast<TargetType>(src[ch][smp]);
    }
    
    // ITA - Il metodo processBlock riceve in input un riferimento al buffer audio e ha il compito di processare l'audio. Il meccanismo generale è quello di chiamare per ogni elemento della catena del filtro la singola processBlock. In particolare vengono calcolati numSamples (numero di campioni da processare), referenceRMS (volume RMS per normalizzare l'uscita dal filtro) e viene creata una copia del buffer in ingresso in formato double per avere più precisione nel calcolo del filtro. Viene poi chiamata la processBlock di ciascun elemento della cascata. In ultimo viene ri-castato il buffer a float e calcolato il volume RMS in uscita. Per equilibrare il volume di uscita rispetto a quello in entrata al filtro vengono calcolati il volume RMS pre-processamento e il volume RMS post-processamento. In ultimo viene applicato un gain a tutti i campioni del buffer calcolato come rapporto tra il volume in ingresso e in uscita.
    // ENG - The processBlock method receives a reference to the audio buffer and is responsible for processing the audio. The general mechanism involves calling the individual processBlock method for each element in the filter chain. Specifically, the method calculates numSamples (the number of samples to be processed) and referenceRMS (the RMS volume to normalize the output from the filter), and creates a copy of the input buffer in double format to increase precision in the filter calculation. The processBlock of each element in the cascade is then called. Finally, the buffer is re-cast to float, and the output RMS volume is calculated. To balance the output volume with the input volume to the filter, the pre-processing and post-processing RMS volumes are calculated. Lastly, a gain is applied to all samples in the buffer, calculated as the ratio between the input and output volumes.
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
    
    // ITA - Il metodo parameterChanged consente il cambio dei valori di bypass e di magnitudine e fase di ciascuno degli zeri e dei poli attivi nella cascata. Il parametro parameterID contiene una stringa id del parametro da cambiare e newValue contiene il valore nuovo da associare.
    // ENG - The parameterChanged method allows the adjustment of bypass values as well as the magnitude and phase of each active zero and pole in the cascade. The parameter parameterID contains a string that identifies the parameter to be changed, and newValue contains the new value to be assigned.
    void parameterChanged (const String& parameterID, float newValue)
    {
        if (parameterID == "BYPASS")
        {
            setUnsetBypass(newValue > 0.5f);
            return;
        }
        
        // I paramterID sono composti nel seguente modo:
        // Primo carattere: Z o P per indicare zero o polo
        // Secondo carattere: M per magnitudine e P per fase
        // Terzo carattere: un numero da 1 a MAX_ORDER che rappresenta il numero in catena del polo o dello zero
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


