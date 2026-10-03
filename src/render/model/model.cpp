#include "render/model/model.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace {
    using vec = DirectX::XMVECTOR;

    constexpr int msample = 10;
    constexpr int mquality = 16;
    constexpr float decival = 1e-6f;

    struct residue {
        std::string chain;
        int seq = 0;
        vec ca = {};
        vec c = {};
        vec n = {};
        vec o = {};
        bool hasca = false;
        bool hasc = false;
        bool hasn = false;
        bool haso = false;
        shape type = shape::coil;
    };

    struct ribbonsample {
        vec center = {};
        vec tangent = {};
        vec horizontal = {};
        vec vertical = {};
        vec colour = {};
        float width = 0.0f;
        float height = 0.0f;
        float power = 2.0f;
        size_t interval = 0;
        float amount = 0.0f;
    };

    struct profile {
        float width;
        float height;
        float power;
    };

    float dot(vec first, vec second){
        return DirectX::XMVectorGetX(DirectX::XMVector3Dot(first, second));
    }

    float length(vec value){
        return DirectX::XMVectorGetX(DirectX::XMVector3Length(value));
    }

    vec cross(vec first, vec second){
        return DirectX::XMVector3Cross(first, second);
    }

    vec unit(vec value){
        return dot(value, value) > decival ? DirectX::XMVector3Normalize(value) : DirectX::XMVectorZero();
    }

    vec point(const atom &value){
        return DirectX::XMVectorSet(value.x, value.y, value.z, 1.0f);
    }

    float smooth(float value){
        value = std::clamp(value, 0.0f, 1.0f);
        return value * value * (3.0f - 2.0f * value);
    }

    profile properties(shape type){
        if (type == shape::sheet) return {0.72f, 0.105f, 5.0f};
        if (type == shape::helix) return {0.56f, 0.18f, 3.6f};
        return {0.19f, 0.19f, 2.0f};
    }

    vec hsv(float hue, float saturation, float value){
        hue = hue - std::floor(hue);
        const float scaled = hue * 6.0f;
        const int section = static_cast<int>(scaled);
        const float part = scaled - section;
        const float low = value * (1.0f - saturation);
        const float fall = value * (1.0f - saturation * part);
        const float rise = value * (1.0f - saturation * (1.0f - part));
        switch (section % 6){
            case 0: return DirectX::XMVectorSet(value, rise, low, 1.0f);
            case 1: return DirectX::XMVectorSet(fall, value, low, 1.0f);
            case 2: return DirectX::XMVectorSet(low, value, rise, 1.0f);
            case 3: return DirectX::XMVectorSet(low, fall, value, 1.0f);
            case 4: return DirectX::XMVectorSet(rise, low, value, 1.0f);
            default: return DirectX::XMVectorSet(value, low, fall, 1.0f);
        }
    }

    vec blend(vec first, vec second, float amount){
        return DirectX::XMVectorLerp(first, second, amount);
    }

    vec safeblend(vec first, vec second, float amount){
        const vec result = blend(first, second, amount);
        return dot(result, result) > decival ? unit(result) : unit(first);
    }

    float knot(vec first, vec second){
        return std::sqrt(std::max(length(second - first), 0.0001f));
    }

    vec knotblend(vec first, vec second, float firsttime, float secondtime, float time){
        const float span = std::max(secondtime - firsttime, 0.0001f);
        return blend(first, second, (time - firsttime) / span);
    }

    vec spline(vec first, vec second, vec third, vec fourth, float amount){
        const float timezero = 0.0f;
        const float timeone = timezero + knot(first, second);
        const float timetwo = timeone + knot(second, third);
        const float timethree = timetwo + knot(third, fourth);
        const float time = timeone + (timetwo - timeone) * amount;

        const vec levelonea = knotblend(first, second, timezero, timeone, time);
        const vec leveloneb = knotblend(second, third, timeone, timetwo, time);
        const vec levelonec = knotblend(third, fourth, timetwo, timethree, time);
        const vec leveltwoa = knotblend(levelonea, leveloneb, timezero, timetwo, time);
        const vec leveltwob = knotblend(leveloneb, levelonec, timeone, timethree, time);
    return knotblend(leveltwoa, leveltwob, timeone, timetwo, time);
    }

    vec guide(const residue &value){
        if (value.haso && value.hasc) return value.o - value.c;
        if (value.hasc) return value.c - value.ca;
        if (value.hasn) return value.n - value.ca;
        return DirectX::XMVectorZero();
    }

    vec perpendicular(vec tangent){
        vec result = cross(tangent, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
        if (dot(result, result) < decival)
            result = cross(tangent, DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
        return unit(result);
    }

    shape edgetype(const std::vector<residue> &run, const ribbonsample &sample){
        const size_t next = std::min(sample.interval + 1, run.size() - 1);
        return run[sample.interval].type == run[next].type ? run[sample.interval].type : shape::coil;
    }

    void appendvertex(modelmesh &mesh, vec position, vec normal, vec colour){
        modelvertex vertex;
        DirectX::XMStoreFloat4(&vertex.position, position);
        DirectX::XMStoreFloat4(&vertex.normal, normal);
        DirectX::XMStoreFloat4(&vertex.colour, colour);
        mesh.vertices.push_back(vertex);
    }

    void appendcap(modelmesh &mesh, const ribbonsample &sample, uint32_t ring, bool start){
        const uint32_t center = static_cast<uint32_t>(mesh.vertices.size());
        const vec normal = sample.tangent * (start ? -1.0f : 1.0f);
        appendvertex(mesh, sample.center, normal, sample.colour);
        for (int horizontal = 0; horizontal < mquality; ++horizontal){
            const uint32_t next = static_cast<uint32_t>((horizontal + 1) % mquality);
            if (start) mesh.indices.insert(mesh.indices.end(), {center, ring + next, ring + static_cast<uint32_t>(horizontal)});
            else mesh.indices.insert(mesh.indices.end(), {center, ring + static_cast<uint32_t>(horizontal), ring + next});
        }
    }

    void finishpiece(modelmesh &mesh, modelpiece &piece){
        piece.countindex = static_cast<uint32_t>(mesh.indices.size()) - piece.firstindex;
        if (piece.countindex == 0) return;
        const auto &start = mesh.vertices[mesh.indices[piece.firstindex]].position;
        piece.minimum = start;
        piece.maximum = start;
        const size_t end = static_cast<size_t>(piece.firstindex) + piece.countindex;
        for (size_t index = piece.firstindex + 1; index < end; ++index){
            const auto &position = mesh.vertices[mesh.indices[index]].position;
            piece.minimum.x = std::min(piece.minimum.x, position.x);
            piece.minimum.y = std::min(piece.minimum.y, position.y);
            piece.minimum.z = std::min(piece.minimum.z, position.z);
            piece.maximum.x = std::max(piece.maximum.x, position.x);
            piece.maximum.y = std::max(piece.maximum.y, position.y);
            piece.maximum.z = std::max(piece.maximum.z, position.z);
        }
    mesh.pieces.push_back(piece);
    }

    std::vector<ribbonsample> makesamples(const std::vector<residue> &run){
        std::vector<ribbonsample> samples;
        samples.reserve((run.size() - 1) * msample + 1);

        for (size_t index = 0; index + 1 < run.size(); ++index){
            const vec second = run[index].ca;
            const vec third = run[index + 1].ca;
            const vec first = index > 0 ? run[index - 1].ca : second - (third - second);
            const vec fourth = index + 2 < run.size() ? run[index + 2].ca : third + (third - second);
            for (int step = 0; step < msample; ++step){
                ribbonsample sample;
                sample.amount = static_cast<float>(step) / msample;
                sample.interval = index;
                sample.center = spline(first, second, third, fourth, sample.amount);
                samples.push_back(sample);
            }
        }

        ribbonsample finalsample;
        finalsample.amount = 1.0f;
        finalsample.interval = run.size() - 2;
        finalsample.center = run.back().ca;
        samples.push_back(finalsample);

        for (size_t index = 0; index < samples.size(); ++index){
            const vec before = samples[index > 0 ? index - 1 : index].center;
            const vec after = samples[index + 1 < samples.size() ? index + 1 : index].center;
            samples[index].tangent = unit(after - before);
        }

        vec previoushorizontal = DirectX::XMVectorZero();
        for (size_t index = 0; index < samples.size(); ++index){
            ribbonsample &sample = samples[index];
            const size_t nextindex = std::min(sample.interval + 1, run.size() - 1);
            const float amount = smooth(sample.amount);
            const profile firstprofile = properties(run[sample.interval].type);
            const profile secondprofile = properties(run[nextindex].type);
            sample.width = firstprofile.width + (secondprofile.width - firstprofile.width) * amount;
            sample.height = firstprofile.height + (secondprofile.height - firstprofile.height) * amount;
            sample.power = firstprofile.power + (secondprofile.power - firstprofile.power) * amount;

            const bool sheetend = run[sample.interval].type == shape::sheet && (run[nextindex].type != shape::sheet || nextindex + 1 == run.size());
            if (sheetend){
                const float arrow = sample.amount < 0.38f ? 1.0f + 0.55f * smooth(sample.amount / 0.38f)
                : 1.55f + (0.18f / properties(shape::sheet).width - 1.55f) * smooth((sample.amount - 0.38f) / 0.62f);
                sample.width = properties(shape::sheet).width * arrow;
                sample.height = properties(shape::sheet).height;
                sample.power = properties(shape::sheet).power;
            }

            const float sequenceamount = (static_cast<float>(sample.interval) + sample.amount) / std::max(1.0f, static_cast<float>(run.size() - 1));
            sample.colour = hsv(sequenceamount * 0.68f, 0.78f, 0.96f);

            vec desired = blend(guide(run[sample.interval]), guide(run[nextindex]), amount);
            desired = desired - sample.tangent * dot(desired, sample.tangent);
            if (dot(desired, desired) > decival) desired = unit(desired);

            if (index == 0){
                previoushorizontal = dot(desired, desired) > decival ? desired : perpendicular(sample.tangent);
            }else {
                vec transported = previoushorizontal - sample.tangent * dot(previoushorizontal, sample.tangent);
                transported = dot(transported, transported) > decival ? unit(transported) : perpendicular(sample.tangent);
                if (dot(desired, desired) > decival){
                    if (dot(desired, transported) < 0.0f) desired = desired * -1.0f;
                    const float guideweight = run[sample.interval].type == shape::sheet ? 0.16f : 0.07f;
                    transported = safeblend(transported, desired, guideweight);
                }
            previoushorizontal = transported;
            }
            sample.horizontal = previoushorizontal;
            sample.vertical = unit(cross(sample.tangent, sample.horizontal));
            sample.horizontal = unit(cross(sample.vertical, sample.tangent));
        }
    return samples;
    }

    void addchain(modelmesh &mesh, const std::vector<residue> &run){
        if (run.size() < 2) return;
        const std::vector<ribbonsample> samples = makesamples(run);
        const uint32_t firstvertex = static_cast<uint32_t>(mesh.vertices.size());

        for (const ribbonsample &sample : samples){
            for (int horizontal = 0; horizontal < mquality; ++horizontal){
                const float angle = DirectX::XM_2PI * static_cast<float>(horizontal) / mquality;
                float sine = 0.0f;
                float cosine = 0.0f;
                DirectX::XMScalarSinCos(&sine, &cosine, angle);

                const float exponent = 2.0f / sample.power;
                const float xunit = std::copysign(std::pow(std::abs(cosine), exponent), cosine);
                const float yunit = std::copysign(std::pow(std::abs(sine), exponent), sine);
                const float x = sample.width * xunit;
                const float y = sample.height * yunit;

                const vec position = sample.center + sample.horizontal * x + sample.vertical * y;
                const float normalx = std::copysign(std::pow(std::abs(xunit), sample.power - 1.0f), xunit) / std::max(sample.width, 0.001f);
                const float normaly = std::copysign(std::pow(std::abs(yunit), sample.power - 1.0f), yunit) / std::max(sample.height, 0.001f);
                const vec normal = unit(sample.horizontal * normalx + sample.vertical * normaly);
                appendvertex(mesh, position, normal, sample.colour);
            }
        }

        size_t firstedge = 1;
        while (firstedge < samples.size()){
            const shape type = edgetype(run, samples[firstedge]);
            size_t lastedge = firstedge;
            while (lastedge + 1 < samples.size() && edgetype(run, samples[lastedge + 1])== type) ++lastedge;

            modelpiece piece;
            piece.firstindex = static_cast<uint32_t>(mesh.indices.size());
            piece.chain = run.front().chain;
            piece.type = type;
            piece.firstresidue = run[samples[firstedge - 1].interval].seq;
            piece.lastresidue = run[std::min(samples[lastedge].interval + 1, run.size() - 1)].seq;

            if (firstedge == 1) appendcap(mesh, samples.front(), firstvertex, true);
            for (size_t edge = firstedge; edge <= lastedge; ++edge){
                const uint32_t previous = firstvertex + static_cast<uint32_t>((edge - 1) * mquality);
                const uint32_t current = firstvertex + static_cast<uint32_t>(edge * mquality);
                for (int horizontal = 0; horizontal < mquality; ++horizontal){
                    const uint32_t next = static_cast<uint32_t>((horizontal + 1) % mquality);
                    mesh.indices.insert(mesh.indices.end(), {
                        previous + static_cast<uint32_t>(horizontal), current + static_cast<uint32_t>(horizontal), current + next,
                        previous + static_cast<uint32_t>(horizontal), current + next, previous + next
                    });
                }
            }
            if (lastedge + 1 == samples.size()){
                const uint32_t lastring = firstvertex + static_cast<uint32_t>((samples.size() - 1) * mquality);
                appendcap(mesh, samples.back(), lastring, false);
            }
        finishpiece(mesh, piece);
        firstedge = lastedge + 1;
        }
    }
}

modelmesh mmodel(const std::vector<atom> &atoms, const std::vector<segment> &segments){
    std::map<std::pair<std::string, int>, residue> residues;
    for (const atom &value : atoms){
        if (std::strcmp(value.t, "ATOM") != 0 || value.s <= 0) continue;
        residue &current = residues[{value.chain, value.s}];
        current.chain = value.chain;
        current.seq = value.s;
        if (std::strcmp(value.d, "CA") == 0){ current.ca = point(value); current.hasca = true; }
        if (std::strcmp(value.d, "C") == 0){ current.c = point(value); current.hasc = true; }
        if (std::strcmp(value.d, "N") == 0){ current.n = point(value); current.hasn = true; }
        if (std::strcmp(value.d, "O") == 0){ current.o = point(value); current.haso = true; }
    }

    for (const segment &current : segments){
        for (int seq = current.first; seq <= current.last; ++seq){
            const auto found = residues.find({current.chain, seq});
            if (found != residues.end()) found->second.type = current.type;
        }
    }

    modelmesh mesh;
    std::vector<residue> run;
    for (const auto &[key, current] : residues){
        if (!current.hasca) continue;
        if (!run.empty()){
            const residue &previous = run.back();
            const vec difference = current.ca - previous.ca;
            if (current.chain != previous.chain || current.seq != previous.seq + 1 || dot(difference, difference) > 25.0f){
                addchain(mesh, run);
                run.clear();
            }
        }
        run.push_back(current);
    }
    addchain(mesh,run);
    return mesh;
}