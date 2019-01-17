t_float tIDLib_taxiDist(t_attributeIdx n, t_float *v1, t_float *v2, t_float *weights)
{
	t_float diff, dist;
	t_attributeIdx i;

	diff=dist=0.0;

	for(i=0; i<n; i++)
	{
		diff = v1[i] - v2[i];
		dist += fabs(diff)*weights[i];
	}
	
	return(dist);
}




static void tabletool_taxi(t_tabletool *x, t_symbol *array1)
{
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayName, garray_class)))
        pd_error(x, "%s: no array named %s", x->x_objSymbol->s_name, x->x_arrayName->s_name);
    else if(!garray_getfloatwords(a, (int *)&x->x_arrayPoints, &x->x_vec))
    	pd_error(x, "%s: bad template for %s", x->x_arrayName->s_name, x->x_objSymbol->s_name);
	else
	{
		t_sampIdx i, array1pts, biggestN;
		t_garray *b;
		t_word *vec1;
		t_float dist, *vecBuffer, *vec1Buffer, *vecWeights;

		if(!(b = (t_garray *)pd_findbyclass(array1, garray_class)))
		{
			pd_error(x, "%s: no array named %s", x->x_objSymbol->s_name, array1->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, (int *)&array1pts, &vec1))
		{
			pd_error(x, "%s: bad template for %s", array1->s_name, x->x_objSymbol->s_name);
			return;
		}

		biggestN = 0;
		
		if(array1pts > x->x_arrayPoints)
			biggestN = array1pts;
		else
			biggestN = x->x_arrayPoints;

		vecBuffer = (t_float *)t_getbytes(biggestN*sizeof(t_float));
		vec1Buffer = (t_float *)t_getbytes(biggestN*sizeof(t_float));
		vecWeights = (t_float *)t_getbytes(biggestN*sizeof(t_float));

		for(i=0; i<biggestN; i++)
		{
			vecBuffer[i] = 0.0;
			vec1Buffer[i] = 0.0;
			// these weights aren't used in tabletool really. Just need to set all weights to 1.0 in order to use tIDLib_ distance functions
			vecWeights[i] = 1.0;
		}

		for(i=0; i<biggestN; i++)
		{
			if(i<x->x_arrayPoints)
				vecBuffer[i] = x->x_vec[i].w_float;
			
			if(i<array1pts)
				vec1Buffer[i] = vec1[i].w_float;
		}
			
		dist = tIDLib_taxiDist(biggestN, vecBuffer, vec1Buffer, vecWeights);

		// free local memory
		t_freebytes(vecBuffer, biggestN * sizeof(t_float));
		t_freebytes(vec1Buffer, biggestN * sizeof(t_float));
		t_freebytes(vecWeights, biggestN * sizeof(t_float));

		outlet_float(x->x_info, dist);
	}
}