
signal = require 'signal'

torch.setdefaulttensortype("torch.FloatTensor")

-- Examples
--
-- t = torch.range(-1,1,0.01)
-- speed = torch.exp(-5*torch.pow(t,2))
-- sal = SpectralArcLength(speed,100.0)
-- sal
-- -1.41403

function diff(a)
	return torch.add(a[{{2,(#a)[1]}}],-a[{{1,(#a)[1]-1}}])
end

function SpectralArcLength(speed,fs,padlevel,fc,amp_th)
	if speed==nil then 
		print("Error! Input at least the movement speed profile for the smoothness calculation")
		return nil
	end
	
	fs=fs or 1/100
	padlevel=padlevel or 4
	fc=fc or 10.0
	amp_th=amp_th or 0.05

	local N=(#speed)[1]
	-- Number of zeros to be padded.
	local Nfft=torch.pow(2,torch.ceil(torch.log(N)/torch.log(2.0))+padlevel)

	local speed_pad=torch.FloatTensor(Nfft):fill(0)
	speed_pad:narrow(1,1,N):copy(speed)
	-- Frequency
	--local freq=torch.range(0,fs,fs/(Nfft-1))
	local freq=torch.linspace(0,fs-(fs/Nfft),Nfft)
	-- Normalized magnitude spectrum
	local speedSpectrum=signal.complex.abs(signal.fft(speed_pad))
	speedSpectrum=speedSpectrum/torch.max(speedSpectrum)
	
	-- Indices to choose only the spectrum within the given cut off frequency fc
	local fc_inx=torch.le(freq,fc)
	freq_sel=freq[fc_inx]
	local speedSpectrum_sel=speedSpectrum[fc_inx]
	
	-- Choose the amplitude threshold based cut off frequency
	local inx=torch.ge(speedSpectrum_sel,amp_th):nonzero()
	freq_sel=freq_sel[{{inx,inx[#inx]}}]
	speedSpectrum_sel=speedSpectrum_sel[{{inx,inx[#inx]}}]

	local new_sal=torch.sum(torch.sqrt(torch.add(
				torch.pow(diff(freq_sel)/(freq_sel[(#freq_sel)[1]]-freq_sel[1]),2)
				,torch.pow(diff(speedSpectrum_sel),2)
			)))
			

	return new_sal,{freq,speedSpectrum},{freq_sel,speedSpectrum_sel}
end
