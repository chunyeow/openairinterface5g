function [q_out] = quantize_q (q_in)

% q_in must be a complex number.

if (real(q_in) > 0)
    if (imag(q_in) > 0)
        if (abs(real(q_in)) > abs(imag(q_in)))
            q_out = 1;
        else
            q_out = 1i;
        end
    else
        if (abs(real(q_in)) > abs(imag(q_in)))
            q_out = 1;
        else
            q_out = -1i;
        end
    end
else
    
    if (imag(q_in) > 0)
        if (abs(real(q_in)) > abs(imag(q_in)))
            q_out = -1;
        else
            q_out = 1i;
        end
    else
        if (abs(real(q_in)) > abs(imag(q_in)))
            q_out = -1;
        else
            q_out = -1i;
        end
    end
end

